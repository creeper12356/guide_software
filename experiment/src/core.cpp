#include "core.h"
#include "mainpage.h"
#include "dependencyinstaller.h"
#include "consoledock.h"
#include "taskmanager.h"
#include "choice.h"
#include "appmodel.h"

Core::Core(QApplication* a):
    QObject(nullptr),
    mApp(a)
{
    //初始化进程和事件循环
    mEventLoop = new TaskEventLoop(this);
    mPubProc = new TaskProcess(this);
    mPubProc->setProgram("bash");
    mPriProc = new TaskProcess(this);
    mPriProc->setProgram("bash");

    //初始化界面
    mMainPage = new MainPage;
    mAptInstaller = new AptInstaller;
    mPyLibInstaller = new PyLibInstaller;

    mAppModel = new AppModel(mMainPage);

    initConnections();

    if(isRunningInDocker()) {
        mMainPage->show();
    }
    else {
        if(mAptInstaller->checkAndInstall() && mPyLibInstaller->checkAndInstall()){
            mMainPage->show();
        }
        else{
            emit quit();
        }
    }
}

Core::~Core()
{
    qDebug() << "~Core()";
    delete mAppModel;
    delete mAptInstaller;
    delete mPyLibInstaller;
    delete mMainPage;
}

void Core::initConnections()
{
    connect(this,&Core::quit,mApp,&QApplication::quit,Qt::QueuedConnection);
    //进程结束后退出事件循环
    connect(mPubProc,SIGNAL(finished(int)),mEventLoop,SLOT(quit()));
    connect(mPriProc,SIGNAL(finished(int)),mEventLoop,SLOT(quit()));
    //清空配置
    connect(mMainPage,&MainPage::clearConfig,this,&Core::clearConfig);
    //清空脚本
    connect(mMainPage,&MainPage::cleanScript,
            this,&Core::cleanScript);
    connect(this,&Core::cleanScriptFinished,mMainPage,&MainPage::cleanScriptFinishedSlot);
    //生成脚本
    connect(mMainPage,&MainPage::genScript,
            this,&Core::genScript);
    connect(this,&Core::genScriptFinished,mMainPage,&MainPage::genScriptFinishedSlot);
    connect(this,&Core::genScriptFailed,mMainPage,&MainPage::genScriptFailedSlot);
    //性能仿真
    connect(mMainPage,&MainPage::simulatePerformance,
            this,&Core::simulatePerformance);
    connect(this,&Core::simulatePerformanceFinished,
            mMainPage,&MainPage::performanceSimulationFinishedSlot);
    connect(this,&Core::simulatePerformanceFailed,
            mMainPage,&MainPage::performanceSimulationFailedSlot);
    //生成温度图
    connect(mMainPage,&MainPage::genHeatMap,
        this,&Core::genHeatMap);
    connect(this,&Core::genHeatMapFinished,
            mMainPage,&MainPage::genHeatMapFinishedSlot);
    //温度场探针
    connect(mMainPage,&MainPage::probe,this,&Core::probe);
    connect(this,&Core::probeResult,mMainPage,&MainPage::displayProbeResult);

    //终止
    connect(mMainPage,&MainPage::terminate,
            this,&Core::terminate);
    //连接终端
    mMainPage->consoleDock()->connectProcess(mPubProc,&cache);

    connect(mPriProc,&QProcess::readyRead,this,[this](){
        cache = mPriProc->readAll();
    });

    //耗时任务的开始和完成
    connect(this,&Core::longTaskStarted,mMainPage,&MainPage::longTaskStartedSlot);
    connect(this,&Core::longTaskFinished,mMainPage,&MainPage::longTaskFinishedSlot);

    //退出逻辑
    connect(mMainPage,&MainPage::quit,this,&Core::checkQuit);
    connect(this,&Core::askQuit,mMainPage,&MainPage::askQuitSlot);
    connect(mMainPage,&MainPage::forceQuit,this,&Core::forceQuit);
}

bool Core::checkConfigured()
{
    return mAppModel->userChoice()->isConfigured();
}

bool Core::checkGenScript()
{
    QDir::setCurrent("TR-09-32-parsec-2.1-alpha-files/");
    QString scriptFormat("%1_%2c_%3.rcS");
    QString script;
    bool res = true;
    for(auto& program: mAppModel->userChoice()->programs){
        script = scriptFormat.arg(program,QString::number(mAppModel->userChoice()->threadNum),
                                  mAppModel->userChoice()->test.toLower());
        if(!QDir::current().exists(script)){
            res = false;
            break;
        }
    }
    QDir::setCurrent("..");
    return res;
}

void Core::clearConfig()
{
    mAppModel->clearUserChoiceAndNotify();
}

void Core::logConsole(const QString &info)
{
    if(mPubProc->isEnabled() && mPriProc->isEnabled()){
        mMainPage->logConsole(info);
    }
}

void Core::logConsoleProgram(const QString &program, const QString &info)
{
    if(mPubProc->isEnabled() && mPriProc->isEnabled()){
        mMainPage->logConsoleProgram(program , info);
    }
}

void Core::cleanScript()
{
    //进入脚本文件夹
    QDir::setCurrent("TR-09-32-parsec-2.1-alpha-files");
    //清空之前生成的脚本
    blockWait(mPubProc,"rm ./*.rcS 2> /dev/null");
    QDir::setCurrent("..");
    emit cleanScriptFinished();
}

void Core::genScript()
{
    //检查前置条件
    if(!checkConfigured()){
        emit genScriptFailed("请先配置，选择基准程序和测试集。");
        return ;
    }
    //进入脚本文件夹
    QDir::setCurrent("TR-09-32-parsec-2.1-alpha-files");
    QString writeScriptCmd = "./writescripts.pl %1 %2";
    //清空之前生成的脚本
    blockWait(mPubProc,"rm ./*.rcS 2>/dev/null");
    for(auto program : mAppModel->userChoice()->programs){
        noBlockWait(mPubProc,
                    writeScriptCmd.arg(program,QString::number(mAppModel->userChoice()->threadNum)),
                    mEventLoop);
    }
    QDir::setCurrent("..");
    emit genScriptFinished();
}

void Core::simulatePerformance()
{
    //检查前置条件
    if(!checkConfigured()){
        emit simulatePerformanceFailed("请先配置，选择基准程序和测试集。");
        return ;
    }
    if(!checkGenScript()){
        emit simulatePerformanceFailed("因部分脚本缺失无法进行仿真，请先生成脚本。");
        return ;
    }

    emit longTaskStarted();
    //清空目录
    QDir::setCurrent("gem5_output");
    blockWait(mPriProc,"rm ./* -rf");
    //新建文件夹
    for(auto program: mAppModel->userChoice()->programs){
        QDir::current().mkdir(program);
    }

    //运行性能仿真
    QDir::setCurrent("..");
    QString simulateCmdFormat =
            "M5_PATH=../full_system_images/ ./build/X86/gem5.opt configs/example/fs.py "
            "--script=../TR-09-32-parsec-2.1-alpha-files/%1_%2c_%3.rcS "
            "--disk-image=x86root-parsec.img "
            "--kernel=x86_64-vmlinux-2.6.28.4-smp --caches "
            "--l2cache --cpu-type 'DerivO3CPU' --maxtime=10";
    QString simulateCmd;
    for(auto program: mAppModel->userChoice()->programs){
        //文件名中测试集均为小写
        if(mPubProc->isEnabled()){
            logConsoleProgram(program,"开始性能仿真...");
        }
        simulateCmd = simulateCmdFormat.arg(
                    program,
                    QString::number(mAppModel->userChoice()->threadNum),
                    mAppModel->userChoice()->test.toLower()
                    );

        mPubProc->setWorkingDirectory("gem5");
        //运行仿真，耗时较长
        noBlockWait(mPubProc,
                    simulateCmd,
                    mEventLoop);
        mPubProc->setWorkingDirectory(".");

        //将输出文件拷贝到对应目标路径
        blockWait(mPriProc,"cp gem5/m5out/* gem5_output/" + program);
        if(mPubProc->isEnabled()){
            logConsoleProgram(program,"[SUCCESS]性能仿真完成.");
        }
    }

    mPubProc->setEnabled(true);
    mPriProc->setEnabled(true);
    mEventLoop->setEnabled(true);

    emit simulatePerformanceFinished();
    emit longTaskFinished();
}

void Core::genHeatMap()
{
    emit longTaskStarted();
    logConsole("开始生成温度图...");
    logConsole("检查性能仿真输出...");
    //准备输入文件夹
    if(QDir::current().exists("McPAT_input")){
        noBlockWait(mPubProc,"rm McPAT_input/* -rf",mEventLoop);
    }
    else {
        QDir::current().mkdir("McPAT_input");
    }
    QDir::setCurrent("gem5_output");
    //仿真结果的所有文件夹列表
    QStringList resultPrograms = QDir::current().entryList(QDir::NoDotAndDotDot | QDir::Dirs);
    if(resultPrograms.empty()){
        logConsole("[FAIL]找不到可用的程序，请检查目录gem5_output。");
    }
    //检查仿真是否成功
    //may buggy here
//    for(auto& program: resultPrograms){
//        if(QDir(program).entryList(QDir::Files).count() != 5){
////            backend buggy here.
////            文件数不为５，仿真失败
//            logProgram(program,"[FAIL]性能仿真结果不完整。终止。");
//            resultPrograms.removeOne(program);
//        }
//    }
    QDir::setCurrent("..");
    //准备温度图文件夹
    blockWait(mPriProc,"mkdir HotSpot_output ; rm HotSpot_output/* -rf");
    blockWait(mPriProc,"mkdir HeatMap ; rm HeatMap/* -rf");

    //处理性能数据
    for(auto& program: resultPrograms){
        logConsoleProgram(program,"分割性能数据...");
        if(!splitGem5Output(program)){
            logConsoleProgram(program,"[FAIL]分割性能仿真结果失败。终止。");
            continue;
        }
        logConsoleProgram(program,"生成xml文件...");
        genXml(program);
        logConsoleProgram(program,"运行McPAT模块...");
        runMcpat(program);
        logConsole("完成!");
        logConsoleProgram(program,"生成ptrace文件...");
        writePtrace(program);
        logConsoleProgram(program,"运行Hotspot模块...");
        runHotspot(program);
        logConsole("完成!");
        logConsoleProgram(program,"生成温度图...");
        drawHeatMap(program);
        logConsoleProgram(program,"[SUCCESS]温度图已成功生成(HeatMap/" + program + ".png). ");
    }

    mPubProc->setEnabled(true);
    mPriProc->setEnabled(true);
    mEventLoop->setEnabled(true);

    //后处理
    //删除除了HotSpot_output之外的所有中间文件夹
    blockWait(mPriProc,"rm McPAT_input McPAT_output HotSpot_input -rf");
    emit genHeatMapFinished();
    emit longTaskFinished();
}

void Core::probe(QString program, qreal probeX, qreal probeY)
    //TODO : optimize algorithm
{
    //TODO : 检查温度图是否生成
    //TODO : read 0.015 from elsewhere
    if(probeX < 0 || probeX > 0.015 || probeY < 0 || probeY > 0.015) {
        qDebug() << "overflow";
        return ;
    }

    QFile steadyFile(QString("HotSpot_output/%1/%1.grid.steady").arg(program));
    const int layer = 0;
    if(!steadyFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return ;
    }
    QTextStream ts(&steadyFile);
    QString line;
    QVector<qreal> temps;
    int count = 0;
    while(!ts.atEnd()) {
        line = ts.readLine();
        if(line[0] == 'L' && count <= layer) {
            ++count;
            continue;
        }
        if(line[0] == 'L' && count > layer) {
            break;
        }
        if(count - 1 == layer) {
            temps.append(QString(line.split(QRegExp("\\s+"))[1]).toDouble());
        }
    }
    qDebug() << "temps size : " << temps.size();
    steadyFile.close();
    const int length = 64;//sqrt(temps.size())
    int col = int(probeX / 0.015 * length);
    int row = int((0.015 - probeY) / 0.015 * length);
    int inx = row * length + col;
    qDebug() << "inx: " << inx;

    emit probeResult(temps[inx],probeX,probeY);
}

void Core::terminate()
{
    logConsole("终止。");
    mPubProc->setEnabled(false);
    mPriProc->setEnabled(false);
    mEventLoop->setEnabled(false);

//ref: https://stackoverflow.com/questions/28830103/qprocesskill-does-not-kill-children-in-linux
    //回收bash的子进程
    QProcess get_childs;
    QStringList get_childs_cmd;
    auto pid = mPubProc->pid();
    if(pid){
        //pid != 0 表示当前有进程正在运行
        get_childs_cmd << "--ppid" << QString::number(pid) << "-o" << "pid" << "--no-heading";
        get_childs.start("ps", get_childs_cmd);
        get_childs.waitForFinished(5000);
        QString childIds = get_childs.readAllStandardOutput();
        qDebug() << "childs: " << childIds;
        childIds.replace('\n', ' ');
        if(!childIds.trimmed().isEmpty()){
            QProcess::execute("kill " + childIds);
        }
        else{
            qDebug() << "no child to kill";
        }
        mPubProc->kill();
        mPubProc->waitForFinished(-1);
    }
    mPriProc->kill();
    mPriProc->waitForFinished(-1);
}

void Core::checkQuit()
{
    if(mPubProc->state() == QProcess::Running || mPriProc->state() == QProcess::Running){
        emit askQuit();
    }
    else{
        emit quit();
    }
}

void Core::forceQuit()
{
    terminate();
    emit quit();
}

bool Core::splitGem5Output(const QString &program)
{
    //准备输出文件夹
    QString mkdirCmd = "mkdir -p McPAT_input/%1";
    mkdirCmd = mkdirCmd.arg(program);
    blockWait(mPriProc,mkdirCmd);

    //分割stats文件
    blockWait(mPubProc,QString("python scripts/split.py gem5_output/%1/stats.txt McPAT_input/").arg(program));
    if(cache == "True\n"){
        //分割成功
        //in folder McPAT_input
        qDebug() << "split success";
        //在McPAT_input中创建文件夹，
        //将分割好的数据移动到该文件夹
        blockWait(mPriProc,QString("cd McPAT_input ; "
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
}

void Core::genXml(const QString &program)
{
    QString genXmlCmd = "python scripts/generateXML.py McPAT_input/%1/3.txt utils/template.xml McPAT_input/%1/3.xml";
    blockWait(mPubProc,genXmlCmd.arg(program));
}

void Core::runMcpat(const QString &program)
{
    //输入的xml文件
    QString xmlFile = "McPAT_input/%1/3.xml";
    xmlFile = xmlFile.arg(program);

    QString mkdirCmd = "mkdir -p McPAT_output/%1";
    mkdirCmd = mkdirCmd.arg(program);
    blockWait(mPriProc,mkdirCmd);
    QString mcpatCmd = "mcpat/mcpat "
                       "-infile %2 "
                       "-print_level 5 "
                       "> McPAT_output/%1/3.txt";
    mcpatCmd = mcpatCmd.arg(program,xmlFile);
    noBlockWait(mPubProc,mcpatCmd,mEventLoop);
}

void Core::writePtrace(const QString &program)
{
    //准备输出文件夹
    QString mkdirCmd = "mkdir HotSpot_input";
    blockWait(mPriProc,mkdirCmd);
    //调用python脚本
    QString writePtraceCmd = "python scripts/writeptrace.py "
                             "utils/ev6.flp "
                             "McPAT_output/%1/3.txt "
                             "HotSpot_input/%1.ptrace";
    writePtraceCmd = writePtraceCmd.arg(program);
    blockWait(mPubProc,writePtraceCmd);
}

void Core::runHotspot(const QString &program)
{
    //创建HotSpot子文件夹
    QString mkdirCmd = "mkdir -p HotSpot_output/%1";
    mkdirCmd = mkdirCmd.arg(program);
    blockWait(mPriProc,mkdirCmd);
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
    mPubProc->setWorkingDirectory("HotSpot-master");
    noBlockWait(mPubProc,hotspotCmd,mEventLoop);
    mPubProc->setWorkingDirectory(".");
}

void Core::drawHeatMap(const QString &program)
{
    //TODO: 检查输出文件夹./HeatMap

    QString heatMapCmd = "python scripts/flpdraw.py "
                         "utils/ev6.flp "
                         "HotSpot_output/%1/%1.grid.steady "
                         "0 "
                         "%1 "
                         "HeatMap/%1";
    heatMapCmd = heatMapCmd.arg(program);
    noBlockWait(mPubProc,heatMapCmd,mEventLoop);
}

