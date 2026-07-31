#ifndef I_RENDERCANVAS_H
#define I_RENDERCANVAS_H

class QWidget;

/**
 * @brief Abstract interface for rendering canvases in LibreCAD.
 * 
 * This interface isolates the rendering backend (e.g. QWidget/QPainter vs OpenGL)
 * from the CAD application logic.
 */
class IRenderCanvas
{
public:
    virtual ~IRenderCanvas() = default;

    /**
     * @brief Get the underlying QWidget for layout integration.
     * @return A pointer to the QWidget surface.
     */
    virtual QWidget* asWidget() = 0;

    /**
     * @brief Request a redraw of the canvas.
     */
    virtual void requestUpdate() = 0;
};

#endif // I_RENDERCANVAS_H
