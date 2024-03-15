#include "inc.h"

#include "logic/core/core.h"

int main(int argc, char *argv[])
{
    // 编译为Release时将此行删除
    QDir::setCurrent("..");

    QApplication a(argc, argv);
    Core core(&a);
    return a.exec();
}
