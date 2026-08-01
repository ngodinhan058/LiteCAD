#ifndef LC_OPENGLRENDERER_H
#define LC_OPENGLRENDERER_H

#include "i_renderer.h"
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <vector>

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
    struct Vertex {
        float x;
        float y;
        uint32_t color;
    };

    void setupShaders();
    void uploadGeometry(const LC_CachedScene* scene);

    QOpenGLShaderProgram m_shaderProgram;
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vbo;

    QMatrix4x4 m_projMatrix;

    uint64_t m_lastRevision = 0;
    int m_vertexCount = 0;
};

#endif // LC_OPENGLRENDERER_H
