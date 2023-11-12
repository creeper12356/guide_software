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
private:
    Core* core = nullptr;
    QProcess* proc = nullptr;
    /* list of packages that are to be installed */
    QStringList pkgList;
public:
    DependencyInstaller(Core* core,QWidget *parent = nullptr);
    ~DependencyInstaller();
    Ui::DependencyInstaller* getUi(){return ui;}
    /*
     * return if all dependencies required are installed,
     * if not , the function will update pkgList as well as GUI
     */
    bool checkDependencies();
private slots:
    /*
     * the slot will be called when user clicked yes(accepted)
     */
    void on_button_box_accepted();
private:
    /*
     * install dependencies with password provided
     */
    void installDependencies(const QString &pwd);
private:
    Ui::DependencyInstaller *ui;
signals:
    /*
     * emitted when all dependencies are installed
     */
    void allInstalled();
};

#endif // DEPENDENCYINSTALLER_H
