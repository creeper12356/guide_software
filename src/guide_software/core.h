#pragma once
#ifndef CORE_H
#define CORE_H
#include "inc.h"
class MainPage;
class DependencyInstaller;
class Core:public QObject
{
    Q_OBJECT
private:
    /*
     * config object
     */
    QJsonObject* config = nullptr;
    /*
     * pointing at currently running application
     */
    QApplication* app = nullptr;

    MainPage* mainPage = nullptr;
    DependencyInstaller* installer = nullptr;
private:
    /*
     * configure software before booting ,
     * according to ./config.json
     */
    void configure();
public:
    Core(QApplication* app);
    ~Core();
private slots:
};

#endif // CORE_H
