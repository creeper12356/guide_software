#include "inc.h"
#include "core.h"
#include "mainpage.h"
#include "ui_mainpage.h"
#include "dependencyinstaller.h"
void Core::configure()
{
    QFile reader("./config.json");
    reader.open(QIODevice::ReadOnly);
    if(!reader.isOpen()){
        qDebug() << "Exception: file not open";
    }
    config = new QJsonObject(QJsonDocument::fromJson(reader.readAll()).object());
    reader.close();
    qDebug() << "config = " << *config;
}

Core::Core(QApplication* a):
    QObject(nullptr),
    app(a)
{
    mainPage = new MainPage(this);
    connect(mainPage,&MainPage::closed,app,&QApplication::quit,Qt::QueuedConnection);
    mainPage->show();
    installer = new DependencyInstaller(this);
    connect(mainPage->getUi()->check_button,&QPushButton::clicked,
            installer,&DependencyInstaller::checkDependencies);
    configure();
//    if(!config->value("isDependencyInstalled").toBool()){
//        //dependency not fully installed
//        installer->exec();
//        qDebug() << "exec finished.";
//    }
}

Core::~Core()
{
    qDebug() << "~Core()";
    delete config;
    delete installer;
    delete mainPage;
}
