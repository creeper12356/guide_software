#ifndef HEATMAPVIEWER_H
#define HEATMAPVIEWER_H
#include "inc.h"
#include "ui/utils/imageviewer.h"

class HeatMapViewer : public ImageViewer
{
    Q_OBJECT
public:
    explicit HeatMapViewer(QWidget* parent = 0);
private:
    QPointF globalToData(const QPoint& pos);
protected:
    void mousePressEvent(QMouseEvent* e) override;
signals:
    void probeTriggered(QString program , qreal x, qreal y);
private:
    QAction* mActionProbe;
};

#endif // HEATMAPVIEWER_H
