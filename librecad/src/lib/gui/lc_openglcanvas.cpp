#include "lc_openglcanvas.h"
#include "../../ui/qg_graphicview.h"
#include "../debug/rs_benchmark.h"
#include "lc_openglrenderer.h"


LC_OpenGLCanvas::LC_OpenGLCanvas(QG_GraphicView* view, QWidget* parent)
    : QOpenGLWidget(parent), m_view(view)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

LC_OpenGLCanvas::~LC_OpenGLCanvas()
{
}

void LC_OpenGLCanvas::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Default to black

    m_renderer = std::make_unique<LC_OpenGLRenderer>();
    m_renderer->initialize();
}

void LC_OpenGLCanvas::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    if (m_renderer) {
        m_renderer->resize(w, h);
    }
}

void LC_OpenGLCanvas::paintGL()
{
    RS_BENCH->beginFrame();

    // Clear the screen to verify OpenGL context is working
    glClear(GL_COLOR_BUFFER_BIT);

    if (m_renderer && m_view) {
        m_renderer->render(m_view->getCachedScene(), 
                           m_view->getOffsetX(), m_view->getOffsetY(), 
                           m_view->getFactor().x, m_view->getFactor().y, 
                           m_view->getWidth(), m_view->getHeight());
    }

    RS_BENCH->setViewportMetrics(m_view->getWidth(), m_view->getHeight(), m_view->getFactor().x);
    RS_BENCH->endFrame();
}
