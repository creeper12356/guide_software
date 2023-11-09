#include "core.h"
#include <QDebug>
#include "mainpage.h"
Core::Core(QApplication* a):
    QObject(nullptr),
    app(a)
{
    mainPage = new MainPage(this);
    mainPage->show();
}

Core::~Core()
{
    delete mainPage;
}

void Core::quit()
{
    app->quit();
}
