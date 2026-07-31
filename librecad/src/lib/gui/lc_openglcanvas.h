#ifndef LC_OPENGLCANVAS_H
#define LC_OPENGLCANVAS_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include "i_rendercanvas.h"

class QG_GraphicView;

class LC_OpenGLCanvas : public QOpenGLWidget, protected QOpenGLFunctions, public IRenderCanvas
{
    Q_OBJECT

public:
    explicit LC_OpenGLCanvas(QG_GraphicView* view, QWidget* parent = nullptr);
    ~LC_OpenGLCanvas() override;

    QWidget* asWidget() override { return this; }
    void requestUpdate() override { update(); }

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    QG_GraphicView* m_view;
};

#endif // LC_OPENGLCANVAS_H
