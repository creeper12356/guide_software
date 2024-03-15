#ifndef PROBEWIDGET_H
#define PROBEWIDGET_H

#include <QWidget>

namespace Ui {
class ProbeWidget;
}

//!探针窗口
class ProbeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProbeWidget(QWidget *parent = 0);
    ~ProbeWidget();
    void setProbeX(qreal x);
    void setProbeY(qreal y);
    void setTemperature(qreal temperature);

private:
    Ui::ProbeWidget *ui;
};

#endif // PROBEWIDGET_H
