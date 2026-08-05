#ifndef LC_GEOMETRYCACHE_H
#define LC_GEOMETRYCACHE_H

#include <vector>
#include <unordered_map>
#include <cstdint>
#include <cstddef>
#include "rs.h"

class LC_CachedScene;

/**
 * @brief Represents API-independent geometry metadata for a single entity.
 */
struct CachedGeometry {
    unsigned int entityId;
    RS2::EntityType primitiveType;
    size_t vertexOffset;
    size_t vertexCount;
    uint64_t processedRevision;
};

/**
 * @brief Vertex format for the GPU buffers.
 */
struct Vertex {
    float x;
    float y;
    uint32_t color;
};

/**
 * @brief CPU-side cache for geometry generated from LC_CachedScene.
 * 
 * Tracks dirtiness via revision numbers and produces flat vertex arrays 
 * suitable for GPU upload. Remains completely independent of OpenGL.
 */
class LC_GeometryCache {
public:
    LC_GeometryCache();
    ~LC_GeometryCache();

    /**
     * @brief Synchronize the cache with the current scene state.
     */
    void synchronize(const LC_CachedScene* scene);

    /**
     * @brief Get the generated line vertices.
     */
    const std::vector<Vertex>& getLineVertices() const { return m_lineVertices; }

    /**
     * @brief Get the ranges of vertices that were modified since last sync.
     * Each pair is (offset, count).
     */
    const std::vector<std::pair<size_t, size_t>>& getDirtyRanges() const { return m_dirtyRanges; }

    /**
     * @brief Clear dirty ranges after they've been consumed (e.g. uploaded to GPU).
     */
    void clearDirtyRanges() { m_dirtyRanges.clear(); }

private:
    void processLineEntity(const class CachedEntity& ent, uint64_t entRev, uint64_t sceneRev);
    void processCircleEntity(const class CachedEntity& ent, uint64_t entRev, uint64_t sceneRev);
    void processArcEntity(const class CachedEntity& ent, uint64_t entRev, uint64_t sceneRev);
    void processEllipseEntity(const class CachedEntity& ent, uint64_t entRev, uint64_t sceneRev);
    
    void pushLineSegment(double x1, double y1, double x2, double y2, uint32_t color);

    void compactBuffer();

    std::vector<Vertex> m_lineVertices;
    std::unordered_map<unsigned int, CachedGeometry> m_metadata;
    std::vector<std::pair<size_t, size_t>> m_dirtyRanges;

    uint64_t m_lastProcessedRevision = 0;
    size_t m_deadSpace = 0;
};

#endif // LC_GEOMETRYCACHE_H
