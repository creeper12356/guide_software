#include "pylibinstaller.h"

PyLibInstaller::PyLibInstaller(Core *core, QEventLoop *&eventLoop, QInputDialog *&pwdDialog, QWidget *parent)
    :DependencyInstaller(core,eventLoop,pwdDialog,parent)
{
    qDebug() << "Python installer!";
    isPasswdNeeded = false;
}

bool PyLibInstaller::checkDependencies()
{
    return DependencyInstaller::checkDependencies();
}

void PyLibInstaller::installDependencies(const QString &pwd)
{
    DependencyInstaller::installDependencies(pwd);
}
