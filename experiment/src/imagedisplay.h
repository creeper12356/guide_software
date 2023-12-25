#ifndef IMAGEDISPLAY_H
#define IMAGEDISPLAY_H
#include "inc.h"
class ImageLabel: public QLabel
{
    friend class ImageDisplay;
private:
    QImage img;
    QString fileName;
public:
    explicit ImageLabel(QWidget *parent = 0);
    void loadFromFile(const QString& fileName);
protected:
    void paintEvent(QPaintEvent* event);
};

class ImageDisplay : public QWidget
{
    Q_OBJECT
private:
    ImageLabel* display;
public:
    explicit ImageDisplay(QWidget *parent = 0);
    void loadFromFile(const QString& fileName);
    const QString& getFileName() const;
};

#endif // IMAGEDISPLAY_H
