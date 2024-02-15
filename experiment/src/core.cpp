#include "core.h"
#include "mainpage.h"
#include "dependencyinstaller.h"
#include "taskmanager.h"
#include "choice.h"
#include "appmodel.h"
#include "compatibility.h"

Core::Core(QApplication* a):
    QObject(nullptr),
    mApp(a)
{
    Compatibility::initialize();

    //初始化进程和事件循环
    mPubProc = new TaskProcess(this);
    mPriProc = new TaskProcess(this);

    //初始化界面
    mMainPage = new MainPage;
    mAptInstaller = new AptInstaller;
    mPyLibInstaller = new PyLibInstaller;

    mAppModel = new AppModel(mMainPage);

    initConnections();

    if(Compatibility::isRunningInDocker()) {
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
    //清空配置
    connect(mMainPage,&MainPage::clearConfig,this,&Core::clearConfig);
    //清空脚本
    connect(mMainPage,&MainPage::cleanScript,
            this,&Core::cleanScript);
    //生成脚本
    connect(mMainPage,&MainPage::genScript,
            this,&Core::genScript);
    //性能仿真
    connect(mMainPage,&MainPage::simulatePerformance,
            this,&Core::simulatePerformance);
    //生成温度图
    connect(mMainPage,&MainPage::genHeatMap,
        this,&Core::genHeatMap);
    //温度场探针
    connect(mMainPage,&MainPage::probe,this,&Core::probe);
    connect(this,&Core::probeResult,mMainPage,&MainPage::displayProbeResult);

    //终止
    connect(mMainPage,&MainPage::terminate,
            this,&Core::terminate);
    //连接终端
    connect(mPubProc,&TaskProcess::stdinUpdated,mMainPage,&MainPage::consoleAppendStdin);
    connect(mPubProc,&TaskProcess::stdoutUpdated,mMainPage,&MainPage::consoleAppendStdout);
    connect(mPubProc,&TaskProcess::stderrUpdated,mMainPage,&MainPage::consoleAppendStderr);

    //耗时任务的开始和完成
    connect(this,&Core::longTaskStarted,mMainPage,&MainPage::longTaskStartedSlot);
    connect(this,&Core::longTaskFinished,mMainPage,&MainPage::longTaskFinishedSlot);

    //退出逻辑
    connect(mMainPage,&MainPage::quit,this,&Core::checkQuit);
    connect(this,&Core::askQuit,mMainPage,&MainPage::askQuitSlot);
    connect(mMainPage,&MainPage::forceQuit,this,&Core::forceQuit);

    //警告和错误
    connect(this,&Core::warning,mMainPage,&MainPage::warning);
    connect(this,&Core::critical,mMainPage,&MainPage::critical);
}

bool Core::checkConfigured()
{
    return mAppModel->userChoice()->isConfigured();
}

bool Core::checkGenScript()
{
    if(!QDir::setCurrent("TR-09-32-parsec-2.1-alpha-files/")) {
        emit critical("找不到目录TR-09-32-parsec-2.1-alpha-file/。");
        //找不到脚本目录
        return false;
    }
    QString scriptFormat("%1_%2c_%3.rcS");
    QString script;
    bool res = true;
    for(auto& program: mAppModel->userChoice()->programs){
        script = scriptFormat.arg(program,
                                  QString::number(mAppModel->userChoice()->threadNum),
                                  mAppModel->userChoice()->test.toLower());
        if(!QDir::current().exists(script)){
            //某程序对应脚本不存在
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
    if(!QDir::setCurrent("TR-09-32-parsec-2.1-alpha-files/")) {
        emit critical("找不到目录TR-09-32-parsec-2.1-alpha-file/。");
        //找不到脚本目录
        return ;
    }
    //清空之前生成的脚本
    mPubProc->blockWaitForFinished("rm ./*.rcS 2> /dev/null");
    QDir::setCurrent("..");
    logConsole("清空脚本完成。");
}

void Core::genScript()
{
    //检查前置条件
    if(!checkConfigured()){
        emit warning("请先配置，选择基准程序和测试集。");
        return ;
    }
    //进入脚本文件夹
    if(!QDir::setCurrent("TR-09-32-parsec-2.1-alpha-files")) {
        emit critical("找不到目录TR-09-32-parsec-2.1-alpha-file/。");
        return ;
    }
    QString writeScriptCmd = "./writescripts.pl %1 %2";
    //清空之前生成的脚本
    mPubProc->blockWaitForFinished("rm ./*.rcS 2>/dev/null");
    for(auto program : mAppModel->userChoice()->programs){
        mPubProc->noBlockWaitForFinished(writeScriptCmd.arg(program,QString::number(mAppModel->userChoice()->threadNum)));
    }
    QDir::setCurrent("..");
    logConsole("脚本已成功生成。");
}

void Core::simulatePerformance()
{
    logConsole("开始性能仿真...");
    //检查前置条件
    if(!checkConfigured()){
        emit warning("请先配置，选择基准程序和测试集。");
        return ;
    }
    if(!checkGenScript()){
        emit warning("因部分脚本缺失无法进行仿真，请先生成脚本。");
        return ;
    }
    if(!QDir("gem5").exists()) {
        emit critical("找不到目录gem5/。");
        return ;
    }

    emit longTaskStarted();

    //准备gem5_output目录
    mPriProc->noBlockWaitForFinished("mkdir gem5_output ; rm gem5_output/* -rf");

    //新建文件夹
    for(auto program: mAppModel->userChoice()->programs){
        QDir("gem5_output").mkdir(program);
    }

    //运行性能仿真
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
        mPubProc->noBlockWaitForFinished(simulateCmd);

        mPubProc->setWorkingDirectory(".");

        //将输出文件拷贝到对应目标路径
        mPriProc->blockWaitForFinished("cp gem5/m5out/* gem5_output/" + program);
        if(mPubProc->isEnabled()){
            logConsoleProgram(program,"[SUCCESS]性能仿真完成.");
        }
    }

    mPubProc->setEnabled(true);
    mPriProc->setEnabled(true);

    logConsole("性能仿真完成。");
    emit longTaskFinished();
}

void Core::genHeatMap()
{
    logConsole("开始生成温度图...");
    //准备输入文件夹
    if(QDir::current().exists("McPAT_input")){
        mPubProc->noBlockWaitForFinished("rm McPAT_input/* -rf");
    }
    else {
        QDir::current().mkdir("McPAT_input");
    }
    //仿真结果的所有文件夹列表
    QStringList resultPrograms = QDir("gem5_output").entryList(QDir::NoDotAndDotDot | QDir::Dirs);
    if(resultPrograms.empty()){
        emit warning("找不到可用的程序，请检查目录gem5_output/。");
        return ;
    }
    //检查关键模块
    if(!QDir("mcpat").exists()) {
        emit critical("找不到目录mcpat/。");
        return ;
    }
    if(!QDir("HotSpot-master").exists()) {
        emit critical("找不到目录HotSpot-master/。");
        return ;
    }

    //检查仿真是否成功
    //TODO : 检查性能仿真文件夹是否有5个文件

    emit longTaskStarted();

    //准备温度图文件夹
    mPriProc->blockWaitForFinished("mkdir HotSpot_output ; rm HotSpot_output/* -rf");
    mPriProc->blockWaitForFinished("mkdir HeatMap ; rm HeatMap/* -rf");

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
        if(QFile(QString("HeatMap/%1.png").arg(program)).exists()) {
            logConsoleProgram(program,"[SUCCESS]温度图已成功生成(HeatMap/" + program + ".png). ");
        }
    }

    mPubProc->setEnabled(true);
    mPriProc->setEnabled(true);

    //删除除了HotSpot_output之外的所有中间文件夹
    mPriProc->blockWaitForFinished("rm McPAT_input McPAT_output HotSpot_input -rf");
    logConsole("生成温度图完成。");
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
    mPriProc->blockWaitForFinished(mkdirCmd);

    //分割stats文件
    //python scripts/split.py gem5_output/{program}/stats.txt McPAT_input/
    QString splitCmd = Compatibility::python() + " scripts/split.py "
                                                 "gem5_output/%1/stats.txt "
                                                 "McPAT_input/";
    splitCmd = splitCmd.arg(program);
    mPubProc->blockWaitForFinished(splitCmd);
    if(mPubProc->cache() == "True\n"){
        //分割成功
        //in folder McPAT_input
        qDebug() << "split success";
        //在McPAT_input中创建文件夹，
        //将分割好的数据移动到该文件夹
        mPriProc->blockWaitForFinished(QString("cd McPAT_input ; "
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
    //python scripts/generateXML.py McPAT_input/{program}/3.txt utils/template.xml McPAT_input/{program}/3.xml
    QString genXmlCmd = Compatibility::python() + " scripts/generateXML.py "
                                                  "McPAT_input/%1/3.txt "
                                                  "utils/template.xml "
                                                  "McPAT_input/%1/3.xml";
    genXmlCmd = genXmlCmd.arg(program);
    mPubProc->blockWaitForFinished(genXmlCmd);
}

void Core::runMcpat(const QString &program)
{
    //输入的xml文件
    QString xmlFile = "McPAT_input/%1/3.xml";
    xmlFile = xmlFile.arg(program);

    QString mkdirCmd = "mkdir -p McPAT_output/%1";
    mkdirCmd = mkdirCmd.arg(program);
    mPriProc->blockWaitForFinished(mkdirCmd);
    QString mcpatCmd = "mcpat/mcpat "
                       "-infile %2 "
                       "-print_level 5 "
                       "> McPAT_output/%1/3.txt";
    mcpatCmd = mcpatCmd.arg(program,xmlFile);
    mPubProc->noBlockWaitForFinished(mcpatCmd);
}

void Core::writePtrace(const QString &program)
{
    //准备输出文件夹
    QString mkdirCmd = "mkdir HotSpot_input";
    mPriProc->blockWaitForFinished(mkdirCmd);
    //调用python脚本
    //python scripts/writeptrace.py utils/ev6.flp McPAT_output/{program}/3.txt HotSpot_input/{program}.ptrace
    QString writePtraceCmd = Compatibility::python() + " scripts/writeptrace.py "
                             "utils/ev6.flp "
                             "McPAT_output/%1/3.txt "
                             "HotSpot_input/%1.ptrace";
    writePtraceCmd = writePtraceCmd.arg(program);
    mPubProc->blockWaitForFinished(writePtraceCmd);
}

void Core::runHotspot(const QString &program)
{
    //创建HotSpot子文件夹
    QString mkdirCmd = "mkdir -p HotSpot_output/%1";
    mkdirCmd = mkdirCmd.arg(program);
    mPriProc->blockWaitForFinished(mkdirCmd);
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
    mPubProc->noBlockWaitForFinished(hotspotCmd);
    mPubProc->setWorkingDirectory(".");
}

void Core::drawHeatMap(const QString &program)
{
    //TODO: 检查输出文件夹./HeatMap

    //python scripts/flpdraw.py utils/ev6.flp HotSpot_output/{program}/{program}.grid.steady 0 {program} HeatMap/{program}
    QString heatMapCmd = Compatibility::python() + " scripts/flpdraw.py "
                         "utils/ev6.flp "
                         "HotSpot_output/%1/%1.grid.steady "
                         "0 "
                         "%1 "
                         "HeatMap/%1";
    heatMapCmd = heatMapCmd.arg(program);
    mPubProc->noBlockWaitForFinished(heatMapCmd);
}

