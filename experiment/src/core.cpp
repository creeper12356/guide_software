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
    _userChoice = new Choice;
    //读取文件配置
    readConfig();
    //初始化窗体
    initPwdDialog();
    mainPage = new MainPage(this);
    //刷新界面
    mainPage->refreshUserChoice(_userChoice);
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
    //写入文件配置
    writeConfig();
    delete _userChoice;
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

    connect(guide,&ChoiceGuide::configureFinished,
            this,&Core::copyUserChoice);
    connect(guide,&ChoiceGuide::configureFinished,
            mainPage,&MainPage::refreshUserChoice);

    connect(mainPage->getUi()->action_conf,&QAction::triggered,
            guide,&ChoiceGuide::show);
    //清理脚本相关
    connect(mainPage->getUi()->action_clean,&QAction::triggered,
            this,&Core::cleanScript);
    connect(this,&Core::cleanScriptFinished,mainPage,&MainPage::scriptCleanedSlot);
    //生成脚本相关
    connect(mainPage->getUi()->action_gen,&QAction::triggered,
            this,&Core::genScript);
    connect(this,&Core::genScriptFinished,mainPage,&MainPage::scriptGeneratedSlot);
    //性能仿真相关
    connect(mainPage->getUi()->action_sim,&QAction::triggered,
            this,&Core::simulatePerformance);
    connect(this,&Core::simulatePerformanceFinished,
            mainPage,&MainPage::performanceSimulationFinishedSlot);
    //生成温度图相关
    connect(mainPage->getUi()->action_temp,&QAction::triggered,
            this,&Core::genTempGraph);
    //终端相关
    connect(proc,&QProcess::readyRead,this,[this](){
        cache = proc->readAll();
        mainPage->getUi()->terminal_reflect->append(QString::fromUtf8(cache));
    });
    connect(mainPage->getUi()->action_terminate,&QAction::triggered,this,[this](){
        proc->terminate();
    });
}

bool Core::checkConfigured()
{
    return !_userChoice->programs.empty();
}

bool Core::checkGenScript()
{
    QDir::setCurrent("TR-09-32-parsec-2.1-alpha-files/");
    QString scriptFormat("%1_%2c_%3.rcS");
    QString script;
    bool res = true;
    for(auto& program:_userChoice->programs){
        script = scriptFormat.arg(program,QString::number(_userChoice->threadNum),_userChoice->test.toLower());
        if(!QDir::current().exists(script)){
            qDebug() << "script " << script << " absent.";
            res = false;
            break;
        }
    }
    QDir::setCurrent("..");
    return res;
}

void Core::writeConfig()
{
    QJsonObject config;
    QJsonObject userChoice;
    userChoice.insert("architecture",_userChoice->architecture);
    userChoice.insert("set",_userChoice->set);
    QJsonArray programs;
    //copy programs
    for(auto& program:_userChoice->programs){
        programs.append(program);
    }
    userChoice.insert("programs",programs);
    userChoice.insert("test",_userChoice->test);
    userChoice.insert("threadNum",_userChoice->threadNum);
    config.insert("userChoice",userChoice);

    QFile writer("config/config.json");
    writer.open(QIODevice::WriteOnly);
    writer.write(QJsonDocument(config).toJson());
    writer.close();
}

void Core::copyUserChoice(const Choice *choice)
{
    *_userChoice = *choice;
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
    //进入脚本文件夹
    QDir::setCurrent("TR-09-32-parsec-2.1-alpha-files");
    //清空之前生成的脚本
    noBlockWait(proc,"rm ./*.rcS",eventLoop);
    QDir::setCurrent("..");
    emit cleanScriptFinished();
}

void Core::genScript()
{
    //检查前置条件
    if(!checkConfigured()){
        QMessageBox::warning(mainPage,"警告","请先配置，选择基准程序和测试集。");
        return ;
    }
    //进入脚本文件夹
    QDir::setCurrent("TR-09-32-parsec-2.1-alpha-files");
    QString writeScriptCmd = "./writescripts.pl %1 %2";
    //清空之前生成的脚本
    noBlockWait(proc,"rm ./*.rcS",eventLoop);
    for(auto program : _userChoice->programs){
        noBlockWait(proc,
                    writeScriptCmd.arg(program,QString::number(_userChoice->threadNum)),
                    eventLoop);
    }
    QDir::setCurrent("..");
    emit genScriptFinished();
}

void Core::simulatePerformance()
{
    //检查前置条件
    if(!checkConfigured()){
        QMessageBox::warning(mainPage,"警告","请先配置，选择基准程序和测试集。");
        return ;
    }
    if(!checkGenScript()){
        QMessageBox::warning(mainPage,"警告","因部分脚本缺失无法进行仿真，请先生成脚本。");
        return ;
    }

    //清空目录
    QDir::setCurrent("gem5_output");
    noBlockWait(proc,"rm ./* -rf",eventLoop);
    //新建文件夹
    for(auto program: _userChoice->programs){
        QDir::current().mkdir(program);
    }

    //进入仿真模块文件夹 并运行性能仿真
    QDir::setCurrent("../gem5");
    QString simulateCmd =
            "M5_PATH=../full_system_images/ ./build/X86/gem5.opt configs/example/fs.py "
            "--script=../TR-09-32-parsec-2.1-alpha-files/%1_%2c_%3.rcS "
            "--disk-image=x86root-parsec.img "
            "--kernel=x86_64-vmlinux-2.6.28.4-smp --caches "
            "--l2cache --cpu-type 'DerivO3CPU' --maxtime=10";

    for(auto program: _userChoice->programs){
        //文件名中测试集均为小写
        //运行仿真，耗时较长
        noBlockWait(proc,
                    simulateCmd.arg(program,QString::number(_userChoice->threadNum),_userChoice->test.toLower()),
                    eventLoop);
        //将输出文件拷贝到对应目标路径
        noBlockWait(proc,"cp m5out/* ../gem5_output/" + program,eventLoop);
    }

    QDir::setCurrent("..");
    emit simulatePerformanceFinished();
}

void Core::genTempGraph()
{
    //准备输入文件夹
    if(QDir::current().exists("McPAT_input")){
        noBlockWait(proc,"rm McPAT_input/* -rf",eventLoop);
    }
    else {
        QDir::current().mkdir("McPAT_input");
    }
    QDir::setCurrent("gem5_output");
    QStringList resultPrograms = QDir::current().entryList(QDir::NoDotAndDotDot | QDir::Dirs);
    //检查仿真是否成功
    for(auto program: resultPrograms){
        if(QDir(program).entryList(QDir::Files).count() != 5){
            //backend buggy here.
            //文件数不为５，仿真失败
            qDebug() << program << " performance simulation fails.";
            resultPrograms.removeOne(program);
        }
    }
    qDebug() << "simulation success list: " << resultPrograms;
    QDir::setCurrent("../McPAT_input");
    for(auto program: resultPrograms){
        //处理性能数据
        noBlockWait(proc,QString("python ../scripts/split.py ../gem5_output/%1/stats.txt .").arg(program),eventLoop);
        if(cache == "True\n"){
            //in folder McPAT_input
            qDebug() << "split success";
            noBlockWait(proc,QString("mkdir %1 && mv ./*.txt %1/").arg(program),eventLoop);
        }
    }
    //TODO : 判断是否分割正确


    QDir::setCurrent("..");
}

void Core::readConfig()
{
    QFile reader("config/config.json");
    reader.open(QIODevice::ReadOnly);
    if(!reader.isOpen()){
        qDebug() << "Exception: file not open";
    }
    QJsonObject config = QJsonDocument::fromJson(reader.readAll()).object();
    reader.close();
    _userChoice->architecture = config["userChoice"].toObject()["architecture"].toString();
    _userChoice->set = config["userChoice"].toObject()["set"].toString();
    //copy programs
    for(auto program:config["userChoice"].toObject()["programs"].toArray()){
        _userChoice->programs.append(program.toString());
    }
    _userChoice->test = config["userChoice"].toObject()["test"].toString();
    _userChoice->threadNum = config["userChoice"].toObject()["threadNum"].toInt();
}
