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
    eventLoop = new QEventLoop(this);
    //TODO: check if eventLoop is deleted properly

    //config by reading from local file
    configure();
    mainPage = new MainPage(this);
    installer = new DependencyInstaller(this,eventLoop,pwdDialog);
    initPwdDialog();

    //connecting signals and slots
    initConnections();


    //check if all requirements are met.
    if(installer->checkAndInstall()){
        mainPage->show();
    }
    else{
        qDebug() << "about to quit.";
        installer->reject();
    }
}

Core::~Core()
{
    qDebug() << "~Core()";
    delete config;
    delete installer;
    delete mainPage;
    delete pwdDialog;
}
void Core::initConnections()
{
    //app backend will quit iff the mainpage is closed
    connect(mainPage,&MainPage::closed,app,&QApplication::quit,Qt::QueuedConnection);
    //if user do not want to install right now , then quit program
    //use QueuedConnection to assert thread safety
    connect(installer->getUi()->button_box,&QDialogButtonBox::rejected,
            app,&QApplication::quit,Qt::QueuedConnection);
        //if user cancels inputing password, installer GUI should switch back.
//    connect(pwdDialog,&QInputDialog::rejected,
//            installer,&DependencyInstaller::switchCheckGUI);
        //report installer error
//    connect(installer,&DependencyInstaller::error,
//            this,&Core::reportError);
}

void Core::initPwdDialog()
{
    pwdDialog = new QInputDialog();
    pwdDialog->setLabelText("我们需要您提供用户的密码：");
    pwdDialog->setTextEchoMode(QLineEdit::Password);
    pwdDialog->setModal(true);
}

void Core::reportError(QString errMsg)
{
    QMessageBox::critical(nullptr,"error",errMsg);
    /*
     * the software will crash.
     */
    app->quit();
}

//void Core::processInstallFinished()
//{
//    qDebug() << "after installation.";
//    installer->switchCheckGUI();
//    if(checkDependencies()){
//        installer->close();
//        mainPage->show();
//    }
//}

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
