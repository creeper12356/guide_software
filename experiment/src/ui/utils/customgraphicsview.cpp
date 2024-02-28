#include "customgraphicsview.h"
#include <QDebug>
CustomGraphicsView::CustomGraphicsView(QWidget *parent) : QGraphicsView(parent)
{
    //设置变换和调整大小锚点为鼠标位置
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
}

void CustomGraphicsView::wheelEvent(QWheelEvent *event) {
    if (QApplication::keyboardModifiers() & Qt::ControlModifier) {
        // Ctrl key is pressed, perform zoom operation
        handleZoom(event);
    } else {
        // Ctrl key is not pressed, perform default scroll operation
        QGraphicsView::wheelEvent(event);
    }
}

void CustomGraphicsView::contextMenuEvent(QContextMenuEvent *event)
{
    //事件传递给父部件
    event->ignore();
}

void CustomGraphicsView::mousePressEvent(QMouseEvent *event)
{
    event->ignore();
}


void CustomGraphicsView::handleZoom(QWheelEvent *event) {
    int delta = event->angleDelta().y();
    // Perform your custom zoom logic here
    // You can use delta to determine the direction of zoom

    // For example, if you want to zoom in/out by a fixed factor:
    double scaleFactor = 1.2; // Adjust as needed
    if (delta > 0) {
        scale(scaleFactor, scaleFactor);
    } else {
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
}
