#include "lc_geometrycache.h"
#include "lc_cachedscene.h"

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
            }
        } else {
            // Existing entity, check revision
            CachedGeometry& cg = it->second;
            if (cg.processedRevision < entRev) {
                // Modified entity
                if (ent.type == RS2::EntityLine) {
                    // Mark old geometry as dead space
                    m_deadSpace += cg.vertexCount;
                    
                    // Generate new geometry
                    processLineEntity(ent, entRev, currentSceneRev);
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

void LC_GeometryCache::compactBuffer() {
    std::vector<Vertex> newVertices;
    newVertices.reserve(m_lineVertices.size() - m_deadSpace);

    for (auto& pair : m_metadata) {
        CachedGeometry& cg = pair.second;
        if (cg.primitiveType == RS2::EntityLine) {
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
