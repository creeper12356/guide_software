#include "imagedisplay.h"

ImageDisplay::ImageDisplay(QWidget *parent) : QWidget(parent)
{
//    this->setMinimumSize(300,300);
//    this->setMaximumSize(800,800);
}

void ImageDisplay::loadFromFile(const QString &fileName)
{
    img.load(fileName);
    update();
}

void ImageDisplay::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.translate(this->geometry().topLeft());
    if(img.isNull()){
        //图片加载失败
        painter.eraseRect(this->geometry());
        return ;
    }
    if(img.width() / this->width() > img.height() / this->height()){
        painter.drawImage(0,0,img.scaledToWidth(width()));
    }
    else{
        painter.drawImage(0,0,img.scaledToHeight(height()));
    }
}


