#pragma once
#ifndef PYLIBINSTALLER_H
#define PYLIBINSTALLER_H
#include "inc.h"
#include "dependencyinstaller.h"

class Core;
class PyLibInstaller : public DependencyInstaller
{
    Q_OBJECT
public:
    PyLibInstaller(Core* core,QWidget* parent = nullptr);
};

#endif // PYLIBINSTALLER_H
