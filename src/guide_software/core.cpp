#include "inc.h"
#include "core.h"
#include "mainpage.h"
#include "ui_mainpage.h"
#include "ui_dependencyinstaller.h"
#include "dependencyinstaller.h"

Core::Core(QApplication* a):
    QObject(nullptr),
    app(a)
{
    configure();
    mainPage = new MainPage(this);
    connect(mainPage,&MainPage::closed,app,&QApplication::quit,Qt::QueuedConnection);
    installer = new DependencyInstaller(this);
    connect(installer->getUi()->button_box,&QDialogButtonBox::rejected,
            app,&QApplication::quit,Qt::QueuedConnection);
    //check if all requirements are met.
    if(!installer->checkDependencies()){
        connect(installer,&DependencyInstaller::allInstalled,
                mainPage,&MainPage::show);
        installer->show();
    }
    else{
        mainPage->show();
    }
}

Core::~Core()
{
    qDebug() << "~Core()";
    delete config;
    delete installer;
    delete mainPage;
}
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
