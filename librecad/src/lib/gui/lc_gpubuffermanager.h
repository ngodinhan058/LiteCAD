#ifndef LC_GPUBUFFERMANAGER_H
#define LC_GPUBUFFERMANAGER_H

#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <vector>
#include <utility>
#include <cstddef>

class LC_GeometryCache;

/**
 * @brief Manages GPU VBOs for the cached geometry.
 * 
 * Takes API-independent vertex data from LC_GeometryCache and safely 
 * uploads dirty ranges to the GPU using QOpenGLBuffer.
 */
class LC_GPUBufferManager : protected QOpenGLFunctions {
public:
    LC_GPUBufferManager();
    ~LC_GPUBufferManager();

    void initialize();
    void synchronize(LC_GeometryCache* cache);

    void bindLineVBO();
    void releaseLineVBO();

private:
    QOpenGLBuffer m_lineVbo;
    size_t m_gpuLineCapacity = 0;
};

#endif // LC_GPUBUFFERMANAGER_H
