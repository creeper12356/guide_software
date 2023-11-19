#include "pylibinstaller.h"

PyLibInstaller::PyLibInstaller(Core *core, QEventLoop *&eventLoop, QInputDialog *&pwdDialog, QWidget *parent)
    :DependencyInstaller(core,eventLoop,pwdDialog,parent)
{
    initCmds();
    isPasswdNeeded = false;
}

void PyLibInstaller::initCmds()
{
    updateCmd = "";
    //this command standard outputs a list of packages not installed
    getNotInstalledCmd =
            "pip3 list | tail -n +3 | cut -f 1 -d ' ' | sort > py_installed.txt "
            "&& "
            "comm py_installed.txt py_requirements.txt -13 "
            "&& "
            "rm py_installed.txt ";
    installCmd =
            "%1pip3 install %2 "
            "2> /dev/null";
}
