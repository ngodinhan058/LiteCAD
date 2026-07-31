#ifndef LC_WIDGETCANVAS_H
#define LC_WIDGETCANVAS_H

#include <QWidget>
#include <QPainter>
#include "i_rendercanvas.h"

class QG_GraphicView;

class LC_WidgetCanvas : public QWidget, public IRenderCanvas
{
    Q_OBJECT

public:
    explicit LC_WidgetCanvas(QG_GraphicView* view, QWidget* parent = nullptr);
    ~LC_WidgetCanvas() override;

    QWidget* asWidget() override { return this; }
    void requestUpdate() override { update(); }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QG_GraphicView* m_view;
};

#endif // LC_WIDGETCANVAS_H
