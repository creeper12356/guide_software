#include "heatmapviewer.h"
#include <QStatusBar>

HeatMapViewer::HeatMapViewer(QWidget *parent)
    : ImageViewer(parent)
{
    mActionProbe = new QAction("探针",this);
    mActionProbe->setCheckable(true);
    toolBar()->addAction(mActionProbe);

    setMouseTracking(true);
}

QPointF HeatMapViewer::globalToData(const QPoint &pos)
{
    //在dpi = 300的温度图矩形如下，单位：像素
    //TODO : 提高可移植性，从matplotlib接口获取
    QRect heatMapRect(324, 180, 1440, 1440);
    QPointF scenePos = view()->mapToScene(view()->mapFromGlobal(pos));

    qreal x = (scenePos.x() - heatMapRect.left()) / heatMapRect.width() * 0.015;
    qreal y = 0.015 - (scenePos.y() - heatMapRect.top()) / heatMapRect.height() * 0.015;

    return QPointF(x,y);
}

void HeatMapViewer::mouseMoveEvent(QMouseEvent *e)
{
    qDebug() << "move event";
    QPointF dataPos = globalToData(e->globalPos());
    statusBar()->showMessage(QString("(%1,%2)").arg(QString::number(dataPos.x()),QString::number(dataPos.y())));
}

void HeatMapViewer::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton && mActionProbe->isChecked()){
        QPointF dataPos = globalToData(e->globalPos());
        emit probeTriggered(QFileInfo(currentFile()).baseName(),dataPos.x(),dataPos.y());
    }
}
