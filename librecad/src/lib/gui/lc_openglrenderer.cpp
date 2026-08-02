#include "lc_openglrenderer.h"
#include "../engine/lc_cachedscene.h"
#include <QMatrix4x4>
#include <vector>
#include "rs_vector.h"

LC_OpenGLRenderer::LC_OpenGLRenderer()
{
}

LC_OpenGLRenderer::~LC_OpenGLRenderer()
{
    m_vao.destroy();
}

void LC_OpenGLRenderer::initialize()
{
    initializeOpenGLFunctions();

    setupShaders();

    m_vao.create();
    m_vao.bind();

    m_bufferManager.initialize();
    m_bufferManager.bindLineVBO();

    // Setup vertex attributes
    // Position (x, y)
    m_shaderProgram.enableAttributeArray(0);
    m_shaderProgram.setAttributeBuffer(0, GL_FLOAT, 0, 2, sizeof(Vertex));
    
    // Color (r, g, b, a) packed as uint32_t (GL_UNSIGNED_BYTE with normalize=true)
    m_shaderProgram.enableAttributeArray(1);
    m_shaderProgram.setAttributeBuffer(1, GL_UNSIGNED_BYTE, offsetof(Vertex, color), 4, sizeof(Vertex));

    m_bufferManager.releaseLineVBO();
    m_vao.release();
}

void LC_OpenGLRenderer::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    m_projMatrix.setToIdentity();
    m_projMatrix.ortho(0, w, h, 0, -1, 1);
}

void LC_OpenGLRenderer::render(const LC_CachedScene* scene, double offsetX, double offsetY, double factorX, double factorY, int viewWidth, int viewHeight)
{
    if (!scene) return;

    m_geometryCache.synchronize(scene);
    m_bufferManager.synchronize(&m_geometryCache);

    size_t lineVertexCount = m_geometryCache.getLineVertices().size();
    if (lineVertexCount == 0) return;

    m_shaderProgram.bind();

    QMatrix4x4 matrix;
    matrix.translate(offsetX, viewHeight - offsetY, 0.0f);
    matrix.scale(factorX, -factorY, 1.0f);

    m_shaderProgram.setUniformValue("transform", m_projMatrix * matrix);

    m_vao.bind();
    glDrawArrays(GL_LINES, 0, lineVertexCount);
    m_vao.release();

    m_shaderProgram.release();
}

void LC_OpenGLRenderer::setupShaders()
{
    const char* vsrc =
        "#version 330 core\n"
        "layout(location = 0) in vec2 position;\n"
        "layout(location = 1) in vec4 color;\n"
        "out vec4 fragColor;\n"
        "uniform mat4 transform;\n"
        "void main() {\n"
        "   gl_Position = transform * vec4(position, 0.0, 1.0);\n"
        "   fragColor = color;\n"
        "}\n";

    const char* fsrc =
        "#version 330 core\n"
        "in vec4 fragColor;\n"
        "out vec4 color;\n"
        "void main() {\n"
        "   color = fragColor;\n"
        "}\n";

    m_shaderProgram.addShaderFromSourceCode(QOpenGLShader::Vertex, vsrc);
    m_shaderProgram.addShaderFromSourceCode(QOpenGLShader::Fragment, fsrc);
    m_shaderProgram.link();
}

