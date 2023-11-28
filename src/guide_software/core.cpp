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
    proc = new QProcess(this);
    proc->setProgram("bash");
    userChoice = new Choice;
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
    delete userChoice;
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

    connect(proc,SIGNAL(finished(int)),eventLoop,SLOT(quit()));
    //report installer error
//    connect(installer,&DependencyInstaller::error,
//            this,&Core::reportError);

    connect(pwdDialog,&QInputDialog::finished,
            eventLoop,&QEventLoop::quit);
    connect(mainPage->getUi()->action_conf,&QAction::triggered,
            guide,&ChoiceGuide::show);

    connect(mainPage->getUi()->action_clean,&QAction::triggered,
            this,&Core::cleanScript);
    connect(this,&Core::scriptCleaned,mainPage,&MainPage::scriptCleanedSlot);

    connect(mainPage->getUi()->action_gen,&QAction::triggered,
            this,&Core::generateScript);
    connect(this,&Core::scriptGenerated,mainPage,&MainPage::scriptGeneratedSlot);

    connect(mainPage->getUi()->action_sim,&QAction::triggered,
            this,&Core::performanceSimulate);
}

bool Core::checkConfigured()
{
    return !userChoice->programs.empty();
}

bool Core::checkScriptGenerated()
{
    QDir oldDir = QDir::current();
    QDir::setCurrent("../experiment/TR-09-32-parsec-2.1-alpha-files/");
    QString scriptFormat("%1_%2c_%3.rcS");
    QString script;
    bool res = true;
    for(auto& program:userChoice->programs){
        script = scriptFormat.arg(program,QString::number(userChoice->threadNum),userChoice->test.toLower());
        qDebug() << "script: " << script;
        if(!QDir::current().exists(script)){
            qDebug() << "script " << script << " does not exist.";
            res = false;
            break;
        }
    }
    QDir::setCurrent(oldDir.path());
    return res;
}

void Core::copyUserChoice(Choice *choice)
{
    *userChoice = *choice;
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

void Core::cleanScript()
{
    //暂存路径
    QDir oldDir = QDir::current();
    QDir::setCurrent("../experiment/TR-09-32-parsec-2.1-alpha-files");
    //清空之前生成的脚本
    proc->setArguments(QStringList() << "-c" << "rm ./*.rcS");
    proc->start();
    eventLoop->exec();
    QDir::setCurrent(oldDir.path());
    emit scriptCleaned();
}

void Core::generateScript()
{
    //检查前置条件
    if(!checkConfigured()){
        QMessageBox::warning(mainPage,"警告","请先配置，选择基准程序和测试集。");
        return ;
    }

    QString writeScriptCmd = "./writescripts.pl %1 %2";
    //暂存路径
    QDir oldDir = QDir::current();
    QDir::setCurrent("../experiment/TR-09-32-parsec-2.1-alpha-files");
    //清空之前生成的脚本
    proc->setArguments(QStringList() << "-c" << "rm ./*.rcS");
    proc->start();
    eventLoop->exec();
    for(auto program : userChoice->programs){
        proc->setArguments(QStringList() << "-c" << writeScriptCmd.arg(program,QString::number(userChoice->threadNum)));
        qDebug() << proc->arguments();
        proc->start();
        eventLoop->exec();
    }
    QDir::setCurrent(oldDir.path());
    emit scriptGenerated();
}

void Core::performanceSimulate()
{
    //检查前置条件
    if(!checkConfigured()){
        QMessageBox::warning(mainPage,"警告","请先配置，选择基准程序和测试集。");
        return ;
    }
    if(!checkScriptGenerated()){
        QMessageBox::warning(mainPage,"警告","因部分脚本缺失无法进行仿真，请先生成脚本。");
        return ;
    }

    qDebug() << "simulate.";
    QDir oldDir = QDir::current();
    QDir::setCurrent("../experiment/gem5_output");
    //清空目录
    proc->setArguments(QStringList() << "-c" << "rm ./* -rf");
    proc->start();
    eventLoop->exec();
    //新建文件夹
    for(auto program: userChoice->programs){
        QDir::current().mkdir(program);
    }
    //进入仿真模块文件夹
    QDir::setCurrent("../gem5");
    //运行性能仿真
    //TODO: 大小写敏感
    QString simulateCmd =
            "M5_PATH=../full_system_images/ ./build/X86/gem5.opt configs/example/fs.py "
            "--script=../TR-09-32-parsec-2.1-alpha-files/%1_%2c_%3.rcS "
            "--disk-image=x86root-parsec.img "
            "--kernel=x86_64-vmlinux-2.6.28.4-smp --caches "
            "--l2cache --cpu-type 'DerivO3CPU' --maxtime=10";
    for(auto program: userChoice->programs){
        proc->setArguments(QStringList() << "-c" <<
                           simulateCmd.arg(program,
                                           QString::number(userChoice->threadNum),
                                           userChoice->test));
        qDebug() << proc->arguments()[1];
        //print process output
        connect(proc,&QProcess::readyRead,this,[this](){
             mainPage->getUi()->textBrowser->append(QString::fromUtf8(proc->readLine()));

        });
        proc->start();
        eventLoop->exec();
        qDebug() << "simulate " << program << " finished..";
    }
    QDir::setCurrent(oldDir.path());
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
