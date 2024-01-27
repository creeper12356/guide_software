#include "inc.h"
#include "core.h"
int main(int argc, char *argv[])
{
    //remove this line before build executable file.
    QDir::setCurrent("..");
    QApplication a(argc, argv);
    Core core(&a);
    return a.exec();
}
