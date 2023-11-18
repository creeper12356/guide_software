#pragma once
#ifndef PYLIBINSTALLER_H
#include "inc.h"
#include "dependencyinstaller.h"
class Core;
class PyLibInstaller: public DependencyInstaller{
public:
    PyLibInstaller(Core* core,
                   QEventLoop*& eventLoop,
                   QInputDialog*& pwdDialog,
                   QWidget *parent = nullptr);
protected:
    bool checkDependencies() override;
    void installDependencies(const QString& pwd) override;
};

#endif // PYLIBINSTALLER_H
