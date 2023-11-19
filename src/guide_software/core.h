#pragma once
#ifndef CORE_H
#define CORE_H
#include "inc.h"
class MainPage;
class DependencyInstaller;
class PyLibInstaller;
class ChoiceGuide;
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
    QEventLoop* eventLoop = nullptr;

    MainPage* mainPage = nullptr;
    DependencyInstaller* installer = nullptr;
    PyLibInstaller* py_installer = nullptr;
    ChoiceGuide* guide = nullptr;
    /*
     * shared by widgets which may ask for password,
     * for example DependencyInstaller
     */
    QInputDialog* pwdDialog = nullptr;


private:
    /*
     * read from ./config.json
     * and configure before starting widgets
     */
    void configure();
public:
    Core(QApplication* app);
    ~Core();
private:
    //init functions
    /* init GUI of password dialog */
    inline void initPwdDialog();
    /* init signals and slots connection */
    inline void initConnections();
private slots:
    void reportError(QString errMsg);
};

#endif // CORE_H
