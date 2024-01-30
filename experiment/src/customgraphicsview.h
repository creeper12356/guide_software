#ifndef CUSTOMGRAPHICSVIEW_H
#define CUSTOMGRAPHICSVIEW_H

#include <QWheelEvent>
#include <QApplication>
#include <QKeyEvent>
#include <QGraphicsView>

class CustomGraphicsView : public QGraphicsView {
public:
    CustomGraphicsView(QWidget* parent = nullptr);

protected:
    void wheelEvent(QWheelEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    void handleZoom(QWheelEvent* event);
};

#endif // CUSTOMGRAPHICSVIEW_H
