#pragma once
#ifndef DEPENDENCYINSTALLER_H
#define DEPENDENCYINSTALLER_H
#include "inc.h"

namespace Ui {
class DependencyInstaller;
}
class Core;
class DependencyInstaller : public QDialog
{
    Q_OBJECT
protected:
    Core* core = nullptr;
    /*
     * pointer to core's member eventLoop
     * used to get rid of GUI freezing
     */
    QEventLoop*& eventLoop;
    /*
     * pointer to core's password dialog
     * used to ask for system's user password
     */
    QInputDialog*& pwdDialog;

    //processes behind widgets
    QProcess* installerProc = nullptr;
    //list of names of unmet dependencies
    QStringList pkgList;

    /*
     * updated after user accpets or rejects it
     */
    bool isAccepted = false;
public:
    DependencyInstaller(Core* core,QEventLoop*& eventLoop,QInputDialog*& pwdDialog,QWidget *parent = nullptr);
    virtual ~DependencyInstaller();
    //getters
    Ui::DependencyInstaller* getUi(){return ui;}
protected:
    /*
     * check if dependencies in requirements.txt
     * are all met
     */
    bool checkDependencies();
    void installDependencies(const QString& pwd);
public:
    /*
     * check and install dependencies
     * called by other classes(for example , Core)
     * return if dependencies are successfully installed
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
     */
    void installProcess(int value);

};

#endif // DEPENDENCYINSTALLER_H
