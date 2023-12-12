#include "imagedisplay.h"

ImageDisplay::ImageDisplay(QWidget *parent) : QWidget(parent)
{
    this->setMinimumSize(300,300);
    this->setMaximumSize(800,800);
}

void ImageDisplay::loadFromFile(const QString &fileName)
{
    img.load(fileName);
    update();
}

void ImageDisplay::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if(img.isNull()){
        //图片加载失败
        painter.eraseRect(this->geometry());
        return ;
    }
    if(img.width() / this->width() > img.height() / this->height()){
        painter.drawImage(this->geometry().topLeft(),img.scaledToWidth(this->width()));
    }
    else{
        painter.drawImage(this->geometry().topLeft(),img.scaledToHeight(this->height()));
    }
}


