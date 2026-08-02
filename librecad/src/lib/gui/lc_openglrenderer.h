#ifndef LC_OPENGLRENDERER_H
#define LC_OPENGLRENDERER_H

#include "i_renderer.h"
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include "lc_geometrycache.h"
#include "lc_gpubuffermanager.h"

class LC_CachedScene;

/**
 * @brief OpenGL implementation of IRenderer.
 * 
 * Handles OpenGL resources, shader binding, and draw calls.
 * Does NOT contain geometry generation or CAD logic.
 */
class LC_OpenGLRenderer : public IRenderer, protected QOpenGLFunctions {
public:
    LC_OpenGLRenderer();
    ~LC_OpenGLRenderer() override;

    void initialize() override;
    void resize(int w, int h) override;
    void render(const LC_CachedScene* scene, double offsetX, double offsetY, double factorX, double factorY, int viewWidth, int viewHeight) override;

private:
    void setupShaders();

    QOpenGLShaderProgram m_shaderProgram;
    QOpenGLVertexArrayObject m_vao;

    LC_GeometryCache m_geometryCache;
    LC_GPUBufferManager m_bufferManager;

    QMatrix4x4 m_projMatrix;
};

#endif // LC_OPENGLRENDERER_H
