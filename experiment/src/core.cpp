#include "core.h"
#include "windows/mainpage.h"
#include "windows/choiceguide.h"
#include "windows/dependencyinstaller.h"
#include "widgets/choicewidget.h"
#include "widgets/consoledock.h"

#include "ui_mainpage.h"
#include "ui_dependencyinstaller.h"
#include "ui_choiceguide.h"

Core::Core(QApplication* a):
    QObject(nullptr),
    app(a)
{
    //初始化进程和事件循环
    eventLoop = new QEventLoop(this);
    pub_proc = new QProcess(this);
    pub_proc->setProgram("bash");
    pri_proc = new QProcess(this);
    pri_proc->setProgram("bash");

    _userChoice = new Choice;
    //读取文件配置
    readConfig();
    //初始化界面
    initPwdDialog();
    mainPage = new MainPage(this);
    mainPage->getChoiceWidget()->refreshUserChoice(_userChoice);
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

bool Core::isProcessRunning() const
{
    return pub_proc->state() == QProcess::Running
            || pri_proc->state() == QProcess::Running;
}
void Core::initConnections()
{
    //退出逻辑
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

    //事件循环相关
    connect(pub_proc,SIGNAL(finished(int)),eventLoop,SLOT(quit()));
    connect(pwdDialog,&QInputDialog::finished,
            eventLoop,&QEventLoop::quit);

    //配置相关
    connect(mainPage->getUi()->action_conf,&QAction::triggered,
            guide,&ChoiceGuide::show);
    connect(guide,&ChoiceGuide::configureFinished,
            this,&Core::copyUserChoice);
    connect(guide,&ChoiceGuide::configureFinished,
            mainPage->getChoiceWidget(),&ChoiceWidget::refreshUserChoice);

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
            this,&Core::genHeatMap);
    //终端相关
    mainPage->getConsoleDock()->connectProcess(pub_proc,&cache);

    connect(pri_proc,&QProcess::readyRead,this,[this](){
        cache = pri_proc->readAll();
    });
    connect(mainPage->getUi()->action_terminate,
            &QAction::triggered,this,&Core::terminate);

    connect(this,&Core::log,mainPage,&MainPage::refreshLog);
    connect(this,&Core::logProgram,mainPage,&MainPage::refreshLogProgram);

    connect(this,&Core::longTaskStarted,mainPage,&MainPage::longTaskStartedSlot);
    connect(this,&Core::longTaskFinished,mainPage,&MainPage::longTaskFinishedSlot);
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
    blockWait(pub_proc,"rm ./*.rcS 2> /dev/null");
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
    blockWait(pub_proc,"rm ./*.rcS 2>/dev/null");
    for(auto program : _userChoice->programs){
        noBlockWait(pub_proc,
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
    emit longTaskStarted();
    //清空目录
    QDir::setCurrent("gem5_output");
    blockWait(pri_proc,"rm ./* -rf");
    //新建文件夹
    for(auto program: _userChoice->programs){
        QDir::current().mkdir(program);
    }

    //进入仿真模块文件夹 并运行性能仿真
    QDir::setCurrent("../gem5");
    QString simulateCmdFormat =
            "M5_PATH=../full_system_images/ ./build/X86/gem5.opt configs/example/fs.py "
            "--script=../TR-09-32-parsec-2.1-alpha-files/%1_%2c_%3.rcS "
            "--disk-image=x86root-parsec.img "
            "--kernel=x86_64-vmlinux-2.6.28.4-smp --caches "
            "--l2cache --cpu-type 'DerivO3CPU' --maxtime=10";
    QString simulateCmd;
    try{
        for(auto program: _userChoice->programs){
            //文件名中测试集均为小写
            emit logProgram(program,"开始性能仿真...");
            simulateCmd = simulateCmdFormat.arg(
                        program,
                        QString::number(_userChoice->threadNum),
                        _userChoice->test.toLower()
                        );
            qDebug() << simulateCmd;
            //运行仿真，耗时较长
            noBlockWait(pub_proc,
                        simulateCmd,
                        eventLoop);
            if(stopFlag){
                throw UserAbort;
            }
            //将输出文件拷贝到对应目标路径
            blockWait(pri_proc,"cp m5out/* ../gem5_output/" + program);
            emit logProgram(program,"[SUCCESS]性能仿真完成.");
        }
    }
    catch(Exception e){
        //abort
        emit log("终止。");
        QDir::setCurrent("..");
        return ;
    }

    QDir::setCurrent("..");
    emit simulatePerformanceFinished();
    emit longTaskFinished();
}

void Core::genHeatMap()
{
    emit longTaskStarted();
    stopFlag = false;
    emit log("开始生成温度图...");
    emit log("检查性能仿真输出...");
    //准备输入文件夹
    if(QDir::current().exists("McPAT_input")){
        noBlockWait(pub_proc,"rm McPAT_input/* -rf",eventLoop);
    }
    else {
        QDir::current().mkdir("McPAT_input");
    }
    QDir::setCurrent("gem5_output");
    //仿真结果的所有文件夹列表
    QStringList resultPrograms = QDir::current().entryList(QDir::NoDotAndDotDot | QDir::Dirs);
    if(resultPrograms.empty()){
        emit log("[FAIL]找不到可用的程序，请检查目录gem5_output。");
    }
    //检查仿真是否成功
//    for(auto& program: resultPrograms){
//        if(QDir(program).entryList(QDir::Files).count() != 5){
////            backend buggy here.
////            文件数不为５，仿真失败
//            emit logProgram(program,"[FAIL]性能仿真结果不完整。终止。");
//            resultPrograms.removeOne(program);
//        }
//    }
    QDir::setCurrent("..");
    //准备温度图文件夹
    blockWait(pri_proc,"mkdir HeatMap ; rm HeatMap/* -rf");


    try{
        //处理性能数据
        for(auto& program: resultPrograms){
            emit log(program + ": 分割性能仿真输出...");
            if(!splitGem5Output(program)){
                emit logProgram(program,"[FAIL]分割性能仿真结果失败。终止。");
                continue;
            }
            emit logProgram(program,"运行McPAT模块...");
            if(!runMcpat(program)){
                throw UserAbort;
            }
            emit log("完成!");
            emit logProgram(program,"生成ptrace文件...");
            writePtrace(program);
            emit logProgram(program,"运行Hotspot模块...");
            if(!runHotspot(program)){
                throw UserAbort;
            }
            emit log("完成!");
            emit logProgram(program,"生成温度图...");
            drawHeatMap(program);
            emit logProgram(program,"[SUCCESS]温度图已成功生成(HeatMap/" + program + ".png). ");
        }
        //删除所有中间文件夹
        blockWait(pri_proc,"rm McPAT_input McPAT_output HotSpot_input HotSpot_output -rf");
        stopFlag = true;
        emit longTaskFinished();
    }
    catch(Exception e){
        emit log("终止。");
        blockWait(pri_proc,"rm McPAT_input McPAT_output HotSpot_input HotSpot_output -rf");
    }
}

void Core::terminate()
{
    qDebug() << "kill.";
    stopFlag = true;
    pub_proc->kill();
    pri_proc->kill();
    emit longTaskFinished();
}

bool Core::splitGem5Output(const QString &program)
{
    //准备输出文件夹
    QString mkdirCmd = "mkdir -p McPAT_input/%1";
    mkdirCmd = mkdirCmd.arg(program);
    blockWait(pri_proc,mkdirCmd);

    //分割stats文件
    blockWait(pub_proc,QString("python scripts/split.py gem5_output/%1/stats.txt McPAT_input/").arg(program));
    if(cache == "True\n"){
        //分割成功
        //in folder McPAT_input
        qDebug() << "split success";
        //在McPAT_input中创建文件夹，
        //将分割好的数据移动到该文件夹
        blockWait(pri_proc,QString("cd McPAT_input ; "
                                   "mkdir %1 ; "
                                   "mv ./*.txt %1/ ; "
                                   "cd ..").arg(program));
        return true;
    }
    else{
        //分割失败
        //TODO: 删除所在文件夹
        return false;
    }
    return !stopFlag;
}

bool Core::runMcpat(const QString &program)
{
    QProcess pr(this);
    pr.setProgram("bash");
    //查找第三段xml文件
    pr.setArguments(QStringList() << "-c" << "ls xml/" + program + "/*3.xml");
    pr.start();
    pr.waitForFinished(-1);
    QString xmlFile(pr.readAll());
    //去除末尾\n
    xmlFile = xmlFile.trimmed();
    QString mkdirCmd = "mkdir -p McPAT_output/%1";
    mkdirCmd = mkdirCmd.arg(program);
    blockWait(pri_proc,mkdirCmd);
    QString mcpatCmd = "mcpat/mcpat "
                       "-infile %2 "
                       "-print_level 5 "
                       "> McPAT_output/%1/3.txt";
    mcpatCmd = mcpatCmd.arg(program,xmlFile);
    noBlockWait(pub_proc,mcpatCmd,eventLoop);
    return !stopFlag;
}

bool Core::writePtrace(const QString &program)
{
    //准备输出文件夹
    QString mkdirCmd = "mkdir HotSpot_input";
    blockWait(pri_proc,mkdirCmd);
    //调用python脚本
    QString writePtraceCmd = "python scripts/writeptrace.py "
                             "utils/ev6.flp "
                             "McPAT_output/%1/3.txt "
                             "HotSpot_input/%1.ptrace";
    writePtraceCmd = writePtraceCmd.arg(program);
    blockWait(pub_proc,writePtraceCmd);
    return !stopFlag;
}

bool Core::runHotspot(const QString &program)
{
    QString mkdirCmd = "mkdir -p HotSpot_output/%1";
    mkdirCmd = mkdirCmd.arg(program);
    blockWait(pri_proc,mkdirCmd);
    QDir::setCurrent("HotSpot-master");
    QString hotspotCmd = "./hotspot "
                         "-c ../utils/example.config "
                         "-f ../utils/ev6.flp "
                         "-p ../HotSpot_input/%1.ptrace "
                         "-materials_file ../utils/example.materials "
                         "-o ../HotSpot_output/%1/%1.ttrace "
                         "-steady_file ../HotSpot_output/%1/%1.steady "
                         "-model_type grid "
                         "-grid_steady_file ../HotSpot_output/%1/%1.grid.steady ";
    hotspotCmd = hotspotCmd.arg(program);
    noBlockWait(pub_proc,hotspotCmd,eventLoop);
    QDir::setCurrent("..");
    return !stopFlag;
}

bool Core::drawHeatMap(const QString &program)
{
    //TODO: 检查输出文件夹./HeatMap

    QString heatMapCmd = "python scripts/flpdraw.py "
                         "utils/ev6.flp "
                         "HotSpot_output/%1/%1.grid.steady "
                         "0 "
                         "%1 "
                         "HeatMap/%1";
    heatMapCmd = heatMapCmd.arg(program);
    blockWait(pub_proc,heatMapCmd);
    return !stopFlag;
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
