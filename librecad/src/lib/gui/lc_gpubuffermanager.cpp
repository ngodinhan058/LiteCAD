#include "lc_gpubuffermanager.h"
#include "../engine/lc_geometrycache.h"
#include <algorithm>

LC_GPUBufferManager::LC_GPUBufferManager()
    : m_lineVbo(QOpenGLBuffer::VertexBuffer)
{
}

LC_GPUBufferManager::~LC_GPUBufferManager() {
    m_lineVbo.destroy();
}

void LC_GPUBufferManager::initialize() {
    initializeOpenGLFunctions();
    m_lineVbo.create();
}

void LC_GPUBufferManager::synchronize(LC_GeometryCache* cache) {
    if (!cache) return;

    const auto& dirtyRanges = cache->getDirtyRanges();
    if (dirtyRanges.empty()) return;

    const auto& vertices = cache->getLineVertices();
    size_t currentVertexCount = vertices.size();
    
    m_lineVbo.bind();

    // Check if we need to resize the GPU buffer
    if (currentVertexCount > m_gpuLineCapacity || m_gpuLineCapacity == 0) {
        // Allocate with 20% extra capacity to avoid frequent reallocations
        m_gpuLineCapacity = currentVertexCount + (currentVertexCount / 5) + 1024;
        
        m_lineVbo.allocate(m_gpuLineCapacity * sizeof(Vertex));
        
        // Full upload since we reallocated
        if (currentVertexCount > 0) {
            m_lineVbo.write(0, vertices.data(), currentVertexCount * sizeof(Vertex));
        }
    } else {
        // Incremental upload
        for (const auto& range : dirtyRanges) {
            size_t offset = range.first;
            size_t count = range.second;
            if (count > 0 && offset + count <= currentVertexCount) {
                m_lineVbo.write(offset * sizeof(Vertex), &vertices[offset], count * sizeof(Vertex));
            }
        }
    }

    m_lineVbo.release();

    cache->clearDirtyRanges();
}

void LC_GPUBufferManager::bindLineVBO() {
    m_lineVbo.bind();
}

void LC_GPUBufferManager::releaseLineVBO() {
    m_lineVbo.release();
}
