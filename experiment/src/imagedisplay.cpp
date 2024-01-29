#include "imagedisplay.h"
ImageDisplay::ImageDisplay(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    mScrollArea = new ImageScrollArea(this);
    display = new ImageLabel(this);

    layout->addWidget(mScrollArea);
    layout->setMargin(0);

    mScrollArea->setWidget(display);

    connect(mScrollArea, &ImageScrollArea::scrollValueChanged, this, &ImageDisplay::onScrollValueChanged);

    // 其他初始化工作
}

void ImageDisplay::loadFromFile(const QString &fileName)
{
    display->loadFromFile(fileName);
}

void ImageDisplay::onScrollValueChanged(int delta)
{
    // 在这里可以根据滚轮的 delta 值进行缩放或其他操作
    // 这里的 delta 值表示滚轮的角度变化，可以根据实际情况进行调整
    // 如果 delta > 0 表示向上滚动，如果 delta < 0 表示向下滚动
    // 可以根据需要进行缩放、滚动等操作
    qDebug() << "delta = " << delta;
}

ImageScrollArea::ImageScrollArea(QWidget *parent) : QScrollArea(parent)
{
    setWidgetResizable(true);
}

void ImageScrollArea::wheelEvent(QWheelEvent *event)
{
    emit scrollValueChanged(event->angleDelta().y());
    QScrollArea::wheelEvent(event);
}

ImageLabel::ImageLabel(QWidget *parent) : QLabel(parent)
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
    if (img.isNull())
    {
        painter.setPen(Qt::red);
        painter.drawText(0, 0, width(), height(), Qt::AlignTop, "找不到文件" + fileName);
        return;
    }

    QPixmap painted;
    if (img.width() / this->width() > img.height() / this->height())
    {
        painted = QPixmap::fromImage(img).scaledToWidth(this->width());
    }
    else
    {
        painted = QPixmap::fromImage(img).scaledToHeight(this->height());
    }

    painter.drawPixmap(0, 0, painted);
}
