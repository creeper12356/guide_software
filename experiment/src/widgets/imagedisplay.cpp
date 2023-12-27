#include "widgets/imagedisplay.h"

ImageDisplay::ImageDisplay(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    QLabel* title = new QLabel(this);
    title->setText("温度图");
    display = new ImageLabel(this);
    layout->addWidget(title);
    layout->addWidget(display);
    layout->setStretchFactor(display,1);
}

void ImageDisplay::loadFromFile(const QString &fileName)
{
    display->loadFromFile(fileName);
}

const QString &ImageDisplay::getFileName() const
{
    return display->fileName;
}

ImageLabel::ImageLabel(QWidget *parent)
    :QLabel(parent)
{
}

void ImageLabel::loadFromFile(const QString &fileName)
{
    this->fileName = fileName;
    img.load(fileName);
    update();
}

void ImageLabel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if(img.isNull()){
        painter.setPen(Qt::red);
        painter.drawText(0,0,width(),height(),Qt::AlignTop,"找不到文件" + fileName);
        return ;
    }
    QPixmap painted;
    if(img.width() / this->width() > img.height() / this->height()){
        painted = QPixmap::fromImage(img).scaledToWidth(this->width());
    }
    else{
        painted = QPixmap::fromImage(img).scaledToHeight(this->height());
    }
    painter.drawPixmap(0,0,painted);
}
