#include "performancecore.h"
#include "logic/core/core.h"
#include "logic/utils/taskmanager.h"
#include "data/model/appmodel.h"
#include "data/utils/choice.h"

PerformanceCore::PerformanceCore(Core *core, AppModel *appModel, TaskProcess *pubProc, TaskProcess *priProc)
    : SubCore(core, appModel, pubProc, priProc)
{
    connect(this,&PerformanceCore::logProgram,core,&Core::logProgram);
    connect(this,&PerformanceCore::simulatePerformanceStarted,core,&Core::longTaskStarted);
    connect(this,&PerformanceCore::simulatePerformanceFinished,core,&Core::longTaskFinished);
}

bool PerformanceCore::checkConfigured()
{
    return mAppModel->userChoice()->isConfigured();
}

bool PerformanceCore::checkGenScript()
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

void PerformanceCore::clearConfig()
{
    mAppModel->clearUserChoiceAndNotify();
    emit log("清空配置完成。");
}

void PerformanceCore::cleanScript()
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
    emit log("清空脚本完成。");
}

void PerformanceCore::genScript()
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
    emit log("脚本已成功生成。");
}

void PerformanceCore::simulatePerformance()
{
    emit log("开始性能仿真...");
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

    emit simulatePerformanceStarted();

    //准备gem5_output目录
    mPriProc->noBlockWaitForFinished("mkdir gem5_output ; rm gem5_output/* -rf");
    //准备performance_data目录
    const QString currentDateTimeStr = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    const QString endOutputDir = QString("performance_data/%1").arg(currentDateTimeStr);
    mPriProc->noBlockWaitForFinished(QString("mkdir -p %1").arg(endOutputDir));

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
            emit logProgram(program,"开始性能仿真...");
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


        if(mPubProc->isEnabled()){
            //将输出文件拷贝到对应目标路径
            mPriProc->blockWaitForFinished("cp gem5/m5out/* gem5_output/" + program);
            mPriProc->blockWaitForFinished(QString("cp gem5_output/%1/stats.txt %2/%1_stats.txt")
                                           .arg(program, endOutputDir));
            emit logProgram(program,"[SUCCESS]性能仿真完成.");
        }
    }

    mPubProc->setEnabled(true);
    mPriProc->setEnabled(true);

    log("性能仿真完成。");
    emit simulatePerformanceFinished();
}
