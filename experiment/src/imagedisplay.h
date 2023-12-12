#ifndef IMAGEDISPLAY_H
#define IMAGEDISPLAY_H
#include "inc.h"

class ImageDisplay : public QWidget
{
    Q_OBJECT
private:
    QImage img;
public:
    explicit ImageDisplay(QWidget *parent = 0);
    void loadFromFile(const QString& fileName) ;
protected:
    void paintEvent(QPaintEvent* event);

signals:

public slots:
};

#endif // IMAGEDISPLAY_H
