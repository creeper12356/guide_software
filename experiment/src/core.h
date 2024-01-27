#ifndef CORE_H
#define CORE_H
#include "inc.h"

/*!
 * \brief 客户端内核
 */
class Core: public QObject
{
    Q_OBJECT

public:
    Core(QApplication* mApp);
    ~Core();

    /*!
     * \brief 初始化信号槽连接
     */
    void initConnections();

    //检查用户是否配置成功
    bool checkConfigured();
    //根据用户配置，检查脚本是否成功生成，若用户没有配置，直接返回true
    bool checkGenScript();

public slots:
    //清理脚本
    void cleanScript();
    //根据用户选择，生成脚本
    void genScript();
    //性能仿真
    void simulatePerformance();
    //生成温度图
    void genHeatMap();
    //终止当前任务
    void terminate();

public:
    //处理gem5输出的性能数据, temporarily useless?
    bool splitGem5Output(const QString& program);
    //运行mcpat模块，处理program对应的xml文件
    void runMcpat(const QString &program);
    //将功耗数据转换为ptrace文件
    void writePtrace(const QString& program);
    //运行hotSpot模块,生成grid.steady文件
    void runHotspot(const QString& program);
    //根据steady文件画温度图
    void drawHeatMap(const QString& program);

    void logConsole(const QString& info);
    void logConsoleProgram(const QString& program, const QString& info);

signals:
    //任务完成的信号
    //清理脚本成功的信号
    void cleanScriptFinished();
    //脚本成功生成的信号
    void genScriptFinished();
    //性能仿真运行成功信号
    void simulatePerformanceFinished();

    //长时间任务开始和结束信号
    void longTaskStarted();
    void longTaskFinished();

signals:
    //退出信号，使用异步通信
    void quit();

private:
    QApplication* mApp = nullptr;
    TaskEventLoop* mEventLoop = nullptr;
    AppModel* mAppModel = nullptr;
    //调用外部程序的进程
    //pub_proc为公有进程，将输出打印到终端，更新cache
    //pri_proc为私有进程，不打印输出，只更新cache
    TaskProcess* mPubProc = nullptr;
    TaskProcess* mPriProc = nullptr;
    //缓冲区,记录上一次readAll操作读到的进程输出
    QByteArray cache;

    MainPage* mMainPage = nullptr;
    AptInstaller* mAptInstaller = nullptr;
    PyLibInstaller* mPyLibInstaller = nullptr;
};

#endif // CORE_H
