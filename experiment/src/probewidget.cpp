#include "probewidget.h"
#include "ui_probewidget.h"

ProbeWidget::ProbeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProbeWidget)
{
    ui->setupUi(this);
}

ProbeWidget::~ProbeWidget()
{
    delete ui;
}

void ProbeWidget::setProbeX(qreal x)
{
    ui->x_input->setText(QString::number(x,'f',7));
}

void ProbeWidget::setProbeY(qreal y)
{
    ui->y_input->setText(QString::number(y,'f',7));
}

void ProbeWidget::setTemperature(qreal temperature)
{
    ui->temp_output->setText(QString::number(temperature,'f',2));
}
