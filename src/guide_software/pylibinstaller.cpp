#include "pylibinstaller.h"

PyLibInstaller::PyLibInstaller(Core *core, QWidget *parent)
    :DependencyInstaller(core,parent)
{
    qDebug() << "new Python-lib installer! ";
}
