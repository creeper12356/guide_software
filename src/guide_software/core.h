#ifndef CORE_H
#define CORE_H
#include <QObject>
#include <QApplication>
class MainPage;
class Core:public QObject
{
    Q_OBJECT
private:
    /*
     * pointing at currently running application
     */
    QApplication* app = nullptr;

    MainPage* mainPage = nullptr;
public:
    Core(QApplication* app);
    ~Core();
    void quit();
};

#endif // CORE_H
