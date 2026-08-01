#include "lc_openglrenderer.h"
#include "../engine/lc_cachedscene.h"
#include <QMatrix4x4>
#include <vector>
#include "rs_vector.h"

LC_OpenGLRenderer::LC_OpenGLRenderer()
    : m_vbo(QOpenGLBuffer::VertexBuffer)
{
}

LC_OpenGLRenderer::~LC_OpenGLRenderer()
{
    m_vao.destroy();
    m_vbo.destroy();
}

void LC_OpenGLRenderer::initialize()
{
    initializeOpenGLFunctions();

    setupShaders();

    m_vao.create();
    m_vao.bind();

    m_vbo.create();
    m_vbo.bind();

    // Setup vertex attributes
    // Position (x, y)
    m_shaderProgram.enableAttributeArray(0);
    m_shaderProgram.setAttributeBuffer(0, GL_FLOAT, 0, 2, sizeof(Vertex));
    
    // Color (r, g, b, a) packed as uint32_t (GL_UNSIGNED_BYTE with normalize=true)
    m_shaderProgram.enableAttributeArray(1);
    m_shaderProgram.setAttributeBuffer(1, GL_UNSIGNED_BYTE, offsetof(Vertex, color), 4, sizeof(Vertex));

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

    if (scene->getRevision() != m_lastRevision) {
        uploadGeometry(scene);
        m_lastRevision = scene->getRevision();
    }

    if (m_vertexCount == 0) return;

    m_shaderProgram.bind();

    QMatrix4x4 matrix;
    matrix.translate(offsetX, viewHeight - offsetY, 0.0f);
    matrix.scale(factorX, -factorY, 1.0f);

    m_shaderProgram.setUniformValue("transform", m_projMatrix * matrix);

    m_vao.bind();
    glDrawArrays(GL_LINES, 0, m_vertexCount);
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

void LC_OpenGLRenderer::uploadGeometry(const LC_CachedScene* scene)
{
    m_vertexCount = 0;
    std::vector<Vertex> vertices;
    LC_Rect fullViewport(RS_Vector(-1e9, -1e9), RS_Vector(1e9, 1e9)); // Fetch all entities
    std::vector<CachedEntity> entities;
    scene->getVisibleEntities(fullViewport, entities);

    for (const auto& ent : entities) {
        if (!ent.visible) continue;
        if (ent.type != RS2::EntityLine) continue;

        // Convert RS_Color to packed uint32 (ABGR for GL_UNSIGNED_BYTE depending on endianness)
        // Little endian typically wants R,G,B,A in memory.
        uint32_t r = ent.color.red();
        uint32_t g = ent.color.green();
        uint32_t b = ent.color.blue();
        uint32_t a = 255;
        uint32_t packedColor = (a << 24) | (b << 16) | (g << 8) | r;

        vertices.push_back({ (float)ent.p1_x, (float)ent.p1_y, packedColor });
        vertices.push_back({ (float)ent.p2_x, (float)ent.p2_y, packedColor });
    }

    m_vertexCount = vertices.size();
    
    if (m_vertexCount > 0) {
        m_vbo.bind();
        m_vbo.allocate(vertices.data(), vertices.size() * sizeof(Vertex));
        m_vbo.release();
    }
}
