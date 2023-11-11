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
public:
    explicit DependencyInstaller(Core* core,QWidget *parent = nullptr);
    ~DependencyInstaller();
    /*TODO:
     * check dependencies required,
     * return a stringlist of absent dependencies.
     * an empty stringlist returned means all dependencies have been installed
     */
    void checkDependencies();
private:
    Ui::DependencyInstaller *ui;
};

#endif // DEPENDENCYINSTALLER_H
