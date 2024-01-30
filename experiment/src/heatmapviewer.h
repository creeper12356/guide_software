#ifndef HEATMAPVIEWER_H
#define HEATMAPVIEWER_H
#include "inc.h"
#include "imageviewer.h"

class HeatMapViewer : public ImageViewer
{
    Q_OBJECT
public:
    explicit HeatMapViewer(QWidget* parent = 0);
protected:
    void mousePressEvent(QMouseEvent* e) override;
};

#endif // HEATMAPVIEWER_H
