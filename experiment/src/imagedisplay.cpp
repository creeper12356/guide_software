#include "imagedisplay.h"

ImageDisplay::ImageDisplay(QWidget *parent) : QWidget(parent)
{

}

void ImageDisplay::loadFromFile(const QString &fileName)
{
    img.load(fileName);
}

void ImageDisplay::paintEvent(QPaintEvent *event)
{
    //TODO: 保持等比例放大
    QPainter painter(this);
    if(img.width() / this->width() > img.height() / this->height()){
        painter.drawImage(this->geometry().topLeft(),img.scaledToWidth(this->width()));
    }
    else{
        painter.drawImage(this->geometry().topLeft(),img.scaledToHeight(this->height()));
    }
}


