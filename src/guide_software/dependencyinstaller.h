#pragma once
#ifndef DEPENDENCYINSTALLER_H
#define DEPENDENCYINSTALLER_H
#include "inc.h"

namespace Ui {
class DependencyInstaller;
}
class Core;
//abstract base class for dependency install dialog
class DependencyInstaller : public QDialog
{
    Q_OBJECT
protected:
    QString updateCmd;
    QString getNotInstalledCmd;
    QString installCmd;
    bool isPasswdNeeded = true;
private:
    Core* core = nullptr;
    /*
     * pointer to core's member eventLoop
     * used to process GUI events
     * used to wait for user to react
     */
    QEventLoop*& eventLoop;
    /*
     * pointer to core's password dialog
     * used to ask for system's user password
     */
    QInputDialog*& pwdDialog;

    /* the process runnning bash */
    QProcess* proc = nullptr;
    /* list of names of unmet dependencies */
    QStringList pkgList;

     /* updated after user accpets or rejects it */
    bool isAccepted = false;

public:
    DependencyInstaller(Core* core,QEventLoop*& eventLoop,QInputDialog*& pwdDialog,QWidget *parent = nullptr);
    virtual ~DependencyInstaller();
    //getters
    Ui::DependencyInstaller* getUi(){return ui;}
protected:
    /*
     * init check and install commands
     */
    virtual void initCmds() = 0;
private:
    /*
     * return if all dependencies in requirements.txt are met
     */
    bool checkDependencies();
    /*
     * install dependencies using password pwd
     */
    void installDependencies(QString pwd);
public:
    /*
     * check and install dependencies
     * called by outer classes(for example , Core)
     * return if dependencies are eventually installed
     */
    bool checkAndInstall();
public slots:
    /* GUI functions */
    virtual void switchCheckGUI();
    virtual void switchInstallGUI();
    void showUnmetDependencies(const QStringList& list);
private slots:
    void setAccepted();
private:
    Ui::DependencyInstaller *ui;
signals:
    /*
     * this signal will be send to core
     * core will pop up a critical dialog(showing errMsg)
     * then quit application
     */
    void error(QString errMsg);
    /*
     * emitted to front end to show install process
     * value is in range [1,100]
     */
    void installProcess(int value);
};

class AptInstaller: public DependencyInstaller
{
public:
    AptInstaller(Core* core,
                 QEventLoop*& eventLoop,
                 QInputDialog*& pwdDialog,
                 QWidget* parent = nullptr);
protected:
    void initCmds() override;
};
class PyLibInstaller: public DependencyInstaller{
public:
    PyLibInstaller(Core* core,
                   QEventLoop*& eventLoop,
                   QInputDialog*& pwdDialog,
                   QWidget *parent = nullptr);
protected:
    void initCmds() override;
};


#endif // DEPENDENCYINSTALLER_H
