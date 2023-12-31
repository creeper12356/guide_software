#include "inc.h"
#include "core.h"
#include <signal.h>
#include <unistd.h>
int main(int argc, char *argv[])
{
    //remove this line before build executable file.
    QDir::setCurrent("..");
    QApplication a(argc, argv);
    qDebug() << "guide_software pgid:" << getpgrp();
    Core core(&a);
    return a.exec();
}
