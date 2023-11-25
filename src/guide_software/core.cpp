#include "core.h"
#include "mainpage.h"
#include "ui_mainpage.h"
#include "dependencyinstaller.h"
#include "ui_dependencyinstaller.h"
#include "choiceguide.h"
#include "ui_choiceguide.h"

Core::Core(QApplication* a):
    QObject(nullptr),
    app(a)
{
    eventLoop = new QEventLoop(this);

    //使用文件配置
    configure();
    //初始化窗体
    initPwdDialog();
    mainPage = new MainPage(this);
    installer = new AptInstaller(this,eventLoop,pwdDialog);
    py_installer = new PyLibInstaller(this,eventLoop,pwdDialog);
    guide = new ChoiceGuide(this);

    initConnections();

    if(installer->checkAndInstall() && py_installer->checkAndInstall()){
        mainPage->show();
    }
    else{
        //TODO: improve logical parts
        qDebug() << "about to quit.";
        app->quit();
    }
}

Core::~Core()
{
    qDebug() << "~Core()";
    delete config;
    delete installer;
    delete py_installer;
    delete mainPage;
    delete pwdDialog;
    delete guide;
}
void Core::initConnections()
{
    //主页面关闭时，软件关闭
    connect(mainPage,&MainPage::closed,app,&QApplication::quit,Qt::QueuedConnection);
    //用户拒绝安装前置软件包，退出软件
    connect(installer->getUi()->button_box,&QDialogButtonBox::rejected,
            app,&QApplication::quit,Qt::QueuedConnection);
    connect(py_installer->getUi()->button_box,&QDialogButtonBox::rejected,
            app,&QApplication::quit,Qt::QueuedConnection);

    //report installer error
//    connect(installer,&DependencyInstaller::error,
//            this,&Core::reportError);

    connect(pwdDialog,&QInputDialog::finished,
            eventLoop,&QEventLoop::quit);

    connect(mainPage->getUi()->conf_button,&QPushButton::clicked,
            guide,&ChoiceGuide::show);

    connect(mainPage->getUi()->gen_button,&QPushButton::clicked,
            this,&Core::writeScripts);
}

void Core::setUserChoice(Choice *choice)
{
    this->userChoice = choice;
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

void Core::writeScripts()
{
    QString writeScriptCmd = "./writescripts.pl %1 %2";
    if(userChoice->programs.empty()){
        qDebug() << "Please configure first.";
    }
    for(auto program : userChoice->programs){
        qDebug() << writeScriptCmd.arg(program,QString::number(userChoice->threadNum));
    }
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
