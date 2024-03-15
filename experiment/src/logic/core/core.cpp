#include "core.h"

#include "logic/core/performancecore.h"
#include "logic/core/heatmapcore.h"
#include "logic/utils/taskmanager.h"
#include "logic/utils/compatibility.h"
#include "data/utils/choice.h"
#include "data/model/appmodel.h"
#include "ui/window/mainpage.h"
#include "ui/window/dependencyinstaller.h"

Core::Core(QApplication* a):
    QObject(nullptr),
    mApp(a)
{
    Compatibility::initialize();

    //初始化进程
    mPubProc = new TaskProcess(this);
    mPriProc = new TaskProcess(this);

    //初始化界面
    mMainPage = new MainPage;
    mAptInstaller = new AptInstaller;
    mPyLibInstaller = new PyLibInstaller;

    mAppModel = new AppModel(mMainPage);

    mPerformanceCore = new PerformanceCore(this , mAppModel , mPubProc, mPriProc);
    mHeatMapCore = new HeatMapCore(this , mAppModel, mPubProc, mPriProc);

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
    connect(mMainPage,&MainPage::clearConfig,mPerformanceCore,&PerformanceCore::clearConfig);
    //清空脚本
    connect(mMainPage,&MainPage::cleanScript,
            mPerformanceCore,&PerformanceCore::cleanScript);
    //生成脚本
    connect(mMainPage,&MainPage::genScript,
            mPerformanceCore,&PerformanceCore::genScript);
    //性能仿真
    connect(mMainPage,&MainPage::simulatePerformance,
            mPerformanceCore,&PerformanceCore::simulatePerformance);
    //生成温度图
    connect(mMainPage,&MainPage::genHeatMap,
            mHeatMapCore,&HeatMapCore::genHeatMap);
    //温度场探针
    connect(mMainPage,&MainPage::probe,mHeatMapCore,&HeatMapCore::probe);
    connect(mHeatMapCore,&HeatMapCore::probeResult,mMainPage,&MainPage::displayProbeResult);

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


void Core::log(const QString &info)
{
    if(mPubProc->isEnabled() && mPriProc->isEnabled()){
        mMainPage->log(info);
    }
}

void Core::logProgram(const QString &program, const QString &info)
{
    if(mPubProc->isEnabled() && mPriProc->isEnabled()){
        mMainPage->logProgram(program , info);
    }
}

bool Core::killChildProcesses(Q_PID pid)
{
    if(!pid) return false;

    QProcess getChildProcess;
    QStringList getChildCommand;
    const int waitTimeOut = 5 * SECOND;

    getChildCommand << "--ppid" << QString::number(pid) << "-o" << "pid" << "--no-heading";
    getChildProcess.start("ps", getChildCommand);
    if(!getChildProcess.waitForFinished(waitTimeOut)) return false;
    QString childIds = getChildProcess.readAllStandardOutput();
    qDebug() << "childs: " << childIds;
    childIds.replace('\n', ' ');

    if(!childIds.trimmed().isEmpty()){
        return QProcess::execute("kill " + childIds) == 0;
    }
    else{
        qDebug() << "no child to kill.";
        return false;
    }
}

void Core::terminate()
{
    log("终止。");
    mPubProc->setEnabled(false);
    mPriProc->setEnabled(false);

    killChildProcesses(mPubProc->pid());
    killChildProcesses(mPriProc->pid());

    mPubProc->kill();
    mPubProc->waitForFinished(-1);
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

