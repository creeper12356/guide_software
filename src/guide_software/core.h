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
    QEventLoop* eventLoop = nullptr;

    MainPage* mainPage = nullptr;
    DependencyInstaller* installer = nullptr;
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
    inline void initPwdDialog();
    inline void initConnections();
private:

private slots:
    void reportError(QString errMsg);
};

#endif // CORE_H
