#include "inc.h"
#include "core.h"
#include "mainpage.h"
#include "ui_mainpage.h"
#include "dependencyinstaller.h"
#include "ui_dependencyinstaller.h"
#include "pylibinstaller.h"

Core::Core(QApplication* a):
    QObject(nullptr),
    app(a)
{
    configure();
    mainPage = new MainPage(this);
    //app backend quit iff the mainpage is closed
    connect(mainPage,&MainPage::closed,app,&QApplication::quit,Qt::QueuedConnection);
    installer = new DependencyInstaller(this);
//    py_installer = new PyLibInstaller(this);

    //connecting signals and slots
    connect(installer->getUi()->button_box,&QDialogButtonBox::rejected,
            app,&QApplication::quit,Qt::QueuedConnection);
    connect(installer,&DependencyInstaller::allInstalled,
                mainPage,&MainPage::show);
    connect(installer,&DependencyInstaller::error,
            this,&Core::reportError);

//    connect(py_installer->getUi()->button_box,&QDialogButtonBox::rejected,
//            app,&QApplication::quit,Qt::QueuedConnection);
//    connect(py_installer,&PyLibInstaller::allInstalled,
//            mainPage,&MainPage::show);
    //check if all requirements are met.
    if(!installer->checkDependencies()){
        installer->show();
//        installer->exec();
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

void Core::reportError(QString errMsg)
{
    QMessageBox::critical(nullptr,"error",errMsg);
    /*
     * the software will crash.
     */
    app->quit();
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
