#include "lc_widgetcanvas.h"
#include "../../ui/qg_graphicview.h"

LC_WidgetCanvas::LC_WidgetCanvas(QG_GraphicView* view, QWidget* parent)
    : QWidget(parent), m_view(view)
{
    // Make canvas transparent to mouse events so the parent (QG_GraphicView) handles them
    setAttribute(Qt::WA_TransparentForMouseEvents);
    
    // Support no background
    setAttribute(Qt::WA_OpaquePaintEvent, false);
}

LC_WidgetCanvas::~LC_WidgetCanvas()
{
}

void LC_WidgetCanvas::paintEvent(QPaintEvent* /*event*/)
{
    if (m_view) {
        QPainter painter(this);
        m_view->renderScene(&painter);
    }
}
