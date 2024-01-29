#include "inc.h"

class ImageLabel : public QLabel
{
public:
    ImageLabel(QWidget *parent = nullptr); void loadFromFile(const QString &fileName);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString fileName;
    QImage img;
};

class ImageScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    ImageScrollArea(QWidget *parent = nullptr);

protected:
    void wheelEvent(QWheelEvent *event) override;

signals:
    void scrollValueChanged(int delta);
};

class ImageDisplay : public QWidget
{
    Q_OBJECT
public:
    ImageDisplay(QWidget *parent = nullptr);
    void loadFromFile(const QString& fileName);

public slots:
    void onScrollValueChanged(int delta);

private:
    ImageScrollArea *mScrollArea;
    ImageLabel *display;

    // 可以添加一些其他变量和函数，如缩放因子、偏移等
};

