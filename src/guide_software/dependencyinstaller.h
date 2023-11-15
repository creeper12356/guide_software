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
    QProcess* proc = nullptr;
    /* subdialog that pops up when asking for password */
    QInputDialog* pwdDialog = nullptr;
    /* list of packages that are to be installed */
    QStringList pkgList;
public:
    DependencyInstaller(Core* core,QWidget *parent = nullptr);
    virtual ~DependencyInstaller();
    Ui::DependencyInstaller* getUi(){return ui;}
    /*
     * return if all dependencies required are installed,
     * if not , the function will update pkgList as well as GUI
     */
    virtual bool checkDependencies();
private slots:
    /* GUI functions */
    virtual void switchCheckGUI();
    virtual void switchInstallGUI();
    /*
     * the slot will be called when user clicked yes(accepted)
     */
    virtual void on_button_box_accepted();

    /*
     * install dependencies with password provided
     */
    virtual void installDependencies(const QString &pwd);
private:
    Ui::DependencyInstaller *ui;
signals:
    /*
     * emitted when all dependencies are installed
     */
    void allInstalled();
    /*
     * this signal will be send to core
     * core will pop up a critical dialog(showing errMsg)
     * then quit application
     */
    void error(QString errMsg);
};

#endif // DEPENDENCYINSTALLER_H
