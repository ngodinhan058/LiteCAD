#include "lc_geometrycache.h"
#include "lc_cachedscene.h"
#include <cmath>

LC_GeometryCache::LC_GeometryCache() = default;
LC_GeometryCache::~LC_GeometryCache() = default;

void LC_GeometryCache::synchronize(const LC_CachedScene* scene) {
    if (!scene) return;

    uint64_t currentSceneRev = scene->getRevision();
    if (currentSceneRev == m_lastProcessedRevision) {
        return;
    }

    const auto& entities = scene->entities();

    for (const auto& pair : entities) {
        const CachedEntity& ent = pair.second;
        if (!ent.visible) continue;

        uint64_t entRev = scene->getEntityRevision(ent.id);

        auto it = m_metadata.find(ent.id);
        if (it == m_metadata.end()) {
            // New entity
            if (ent.type == RS2::EntityLine) {
                processLineEntity(ent, entRev, currentSceneRev);
            } else if (ent.type == RS2::EntityCircle) {
                processCircleEntity(ent, entRev, currentSceneRev);
            } else if (ent.type == RS2::EntityArc) {
                processArcEntity(ent, entRev, currentSceneRev);
            } else if (ent.type == RS2::EntityEllipse) {
                processEllipseEntity(ent, entRev, currentSceneRev);
            }
        } else {
            // Existing entity, check revision
            CachedGeometry& cg = it->second;
            if (cg.processedRevision < entRev) {
                // Modified entity
                if (ent.type == RS2::EntityLine || ent.type == RS2::EntityCircle || 
                    ent.type == RS2::EntityArc || ent.type == RS2::EntityEllipse) {
                    // Mark old geometry as dead space
                    m_deadSpace += cg.vertexCount;
                    
                    // Generate new geometry
                    if (ent.type == RS2::EntityLine) processLineEntity(ent, entRev, currentSceneRev);
                    else if (ent.type == RS2::EntityCircle) processCircleEntity(ent, entRev, currentSceneRev);
                    else if (ent.type == RS2::EntityArc) processArcEntity(ent, entRev, currentSceneRev);
                    else if (ent.type == RS2::EntityEllipse) processEllipseEntity(ent, entRev, currentSceneRev);
                }
            } else {
                // Unchanged entity, just update processedRevision to current scene revision
                // so we don't sweep it
                cg.processedRevision = currentSceneRev;
            }
        }
    }

    // Sweep: find entities that were removed from the scene
    std::vector<unsigned int> removedIds;
    for (auto& pair : m_metadata) {
        if (pair.second.processedRevision < currentSceneRev) {
            removedIds.push_back(pair.first);
        }
    }

    for (unsigned int id : removedIds) {
        m_deadSpace += m_metadata[id].vertexCount;
        m_metadata.erase(id);
    }

    // Compaction threshold (e.g. 20% dead space)
    if (m_lineVertices.size() > 0 && (float)m_deadSpace / m_lineVertices.size() > 0.2f) {
        compactBuffer();
    }

    m_lastProcessedRevision = currentSceneRev;
}

void LC_GeometryCache::processLineEntity(const CachedEntity& ent, uint64_t entRev, uint64_t sceneRev) {
    CachedGeometry cg;
    cg.entityId = ent.id;
    cg.primitiveType = RS2::EntityLine;
    cg.vertexOffset = m_lineVertices.size();
    cg.vertexCount = 2;
    cg.processedRevision = sceneRev;

    uint32_t r = ent.color.red();
    uint32_t g = ent.color.green();
    uint32_t b = ent.color.blue();
    uint32_t color = (255 << 24) | (b << 16) | (g << 8) | r;

    m_lineVertices.push_back({(float)ent.p1_x, (float)ent.p1_y, color});
    m_lineVertices.push_back({(float)ent.p2_x, (float)ent.p2_y, color});

    m_metadata[ent.id] = cg;
    m_dirtyRanges.push_back({cg.vertexOffset, cg.vertexCount});
}

void LC_GeometryCache::pushLineSegment(double x1, double y1, double x2, double y2, uint32_t color) {
    m_lineVertices.push_back({(float)x1, (float)y1, color});
    m_lineVertices.push_back({(float)x2, (float)y2, color});
}

void LC_GeometryCache::processCircleEntity(const CachedEntity& ent, uint64_t entRev, uint64_t sceneRev) {
    CachedGeometry cg;
    cg.entityId = ent.id;
    cg.primitiveType = RS2::EntityCircle;
    cg.vertexOffset = m_lineVertices.size();
    cg.processedRevision = sceneRev;

    uint32_t r = ent.color.red();
    uint32_t g = ent.color.green();
    uint32_t b = ent.color.blue();
    uint32_t color = (255 << 24) | (b << 16) | (g << 8) | r;

    const int segments = 64;
    double cx = ent.p1_x;
    double cy = ent.p1_y;
    double rad = ent.radius;

    double prevX = cx + rad;
    double prevY = cy;

    for (int i = 1; i <= segments; ++i) {
        double angle = (2.0 * M_PI * i) / segments;
        double x = cx + rad * std::cos(angle);
        double y = cy + rad * std::sin(angle);
        pushLineSegment(prevX, prevY, x, y, color);
        prevX = x;
        prevY = y;
    }

    cg.vertexCount = m_lineVertices.size() - cg.vertexOffset;
    m_metadata[ent.id] = cg;
    m_dirtyRanges.push_back({cg.vertexOffset, cg.vertexCount});
}

void LC_GeometryCache::processArcEntity(const CachedEntity& ent, uint64_t entRev, uint64_t sceneRev) {
    CachedGeometry cg;
    cg.entityId = ent.id;
    cg.primitiveType = RS2::EntityArc;
    cg.vertexOffset = m_lineVertices.size();
    cg.processedRevision = sceneRev;

    uint32_t r = ent.color.red();
    uint32_t g = ent.color.green();
    uint32_t b = ent.color.blue();
    uint32_t color = (255 << 24) | (b << 16) | (g << 8) | r;

    double cx = ent.p1_x;
    double cy = ent.p1_y;
    double rad = ent.radius;
    double a1 = ent.angle1;
    double a2 = ent.angle2;

    double sweep = a2 - a1;
    while (sweep <= 0.0) sweep += 2.0 * M_PI;

    int segments = std::max(8, (int)(64.0 * (sweep / (2.0 * M_PI))));

    double prevX = cx + rad * std::cos(a1);
    double prevY = cy + rad * std::sin(a1);

    for (int i = 1; i <= segments; ++i) {
        double angle = a1 + sweep * ((double)i / segments);
        double x = cx + rad * std::cos(angle);
        double y = cy + rad * std::sin(angle);
        pushLineSegment(prevX, prevY, x, y, color);
        prevX = x;
        prevY = y;
    }

    cg.vertexCount = m_lineVertices.size() - cg.vertexOffset;
    m_metadata[ent.id] = cg;
    m_dirtyRanges.push_back({cg.vertexOffset, cg.vertexCount});
}

void LC_GeometryCache::processEllipseEntity(const CachedEntity& ent, uint64_t entRev, uint64_t sceneRev) {
    CachedGeometry cg;
    cg.entityId = ent.id;
    cg.primitiveType = RS2::EntityEllipse;
    cg.vertexOffset = m_lineVertices.size();
    cg.processedRevision = sceneRev;

    uint32_t r = ent.color.red();
    uint32_t g = ent.color.green();
    uint32_t b = ent.color.blue();
    uint32_t color = (255 << 24) | (b << 16) | (g << 8) | r;

    double cx = ent.p1_x;
    double cy = ent.p1_y;
    double majorX = ent.p2_x;
    double majorY = ent.p2_y;
    double ratio = ent.radius;
    double a1 = ent.angle1;
    double a2 = ent.angle2;
    bool reversed = ent.reversed;

    double majorLen = std::sqrt(majorX * majorX + majorY * majorY);
    double minorLen = majorLen * ratio;
    double rotAngle = std::atan2(majorY, majorX);

    double sweep = a2 - a1;
    if (!reversed) {
        while (sweep <= 0.0) sweep += 2.0 * M_PI;
    } else {
        while (sweep >= 0.0) sweep -= 2.0 * M_PI;
    }

    // Whole ellipse
    if (std::abs(std::abs(sweep) - 2.0 * M_PI) < 1e-6 || sweep == 0.0) {
        sweep = reversed ? -2.0 * M_PI : 2.0 * M_PI;
    }

    int segments = 64;

    auto getEllipsePoint = [&](double param) {
        double px = majorLen * std::cos(param);
        double py = minorLen * std::sin(param);
        double rx = px * std::cos(rotAngle) - py * std::sin(rotAngle);
        double ry = px * std::sin(rotAngle) + py * std::cos(rotAngle);
        return std::make_pair(cx + rx, cy + ry);
    };

    auto prev = getEllipsePoint(a1);

    for (int i = 1; i <= segments; ++i) {
        double param = a1 + sweep * ((double)i / segments);
        auto pt = getEllipsePoint(param);
        pushLineSegment(prev.first, prev.second, pt.first, pt.second, color);
        prev = pt;
    }

    cg.vertexCount = m_lineVertices.size() - cg.vertexOffset;
    m_metadata[ent.id] = cg;
    m_dirtyRanges.push_back({cg.vertexOffset, cg.vertexCount});
}

void LC_GeometryCache::compactBuffer() {
    std::vector<Vertex> newVertices;
    newVertices.reserve(m_lineVertices.size() - m_deadSpace);

    for (auto& pair : m_metadata) {
        CachedGeometry& cg = pair.second;
        if (cg.vertexCount > 0) {
            size_t newOffset = newVertices.size();
            for (size_t i = 0; i < cg.vertexCount; ++i) {
                newVertices.push_back(m_lineVertices[cg.vertexOffset + i]);
            }
            cg.vertexOffset = newOffset;
        }
    }

    m_lineVertices = std::move(newVertices);
    m_deadSpace = 0;

    // After compaction, the whole buffer is dirty
    m_dirtyRanges.clear();
    m_dirtyRanges.push_back({0, m_lineVertices.size()});
}
