#pragma once
#ifndef CORE_H
#define CORE_H
#include "inc.h"
class MainPage;
class DependencyInstaller;
class PyLibInstaller;
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
//    PyLibInstaller* py_installer = nullptr;
    QInputDialog* pwdDialog = nullptr;

    //processes behind widgets
    QProcess* installerProc = nullptr;
    //list of names of unmet dependencies
    QStringList pkgList;
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
    bool checkDependencies();
    void installDependencies(const QString& pwd);
private slots:
    void reportError(QString errMsg);
    void askForPwd();
signals:
    /*
     * send to dependency installer
     * to show unmet dependencies
     * in GUI
     */
    void unmetDependencies(const QStringList& list);
    /*
     * send to dependency installer
     * to show installing process
     * process is between 0 and 100
     */
    void installProcess(int process);
};

#endif // CORE_H
