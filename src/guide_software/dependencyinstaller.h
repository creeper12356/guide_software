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
public:
    DependencyInstaller(Core* core,QWidget *parent = nullptr);
    virtual ~DependencyInstaller();
    Ui::DependencyInstaller* getUi(){return ui;}
public slots:
    /* GUI functions */
    virtual void switchCheckGUI();
    virtual void switchInstallGUI();
    void showUnmetDependencies(const QStringList& list);
private:
    Ui::DependencyInstaller *ui;
signals:
    /*
     * this signal will be send to core
     * core will pop up a critical dialog(showing errMsg)
     * then quit application
     */
    void error(QString errMsg);
};

#endif // DEPENDENCYINSTALLER_H
