#pragma once
#ifndef CORE_H
#define CORE_H
#include "inc.h"
class Core:public QObject
{
    Q_OBJECT
private:
    QApplication* app = nullptr;
    QEventLoop* eventLoop = nullptr;
    //调用外部程序的进程
    //pub_proc为公有进程，将输出打印到终端，更新cache
    //pri_proc为私有进程，不打印输出，只更新cache
    TaskProcess* pub_proc = nullptr;
    TaskProcess* pri_proc = nullptr;
    //缓冲区,记录上一次readAll操作读到的进程输出
    QByteArray cache;

    //窗口
    MainPage* mainPage = nullptr;
    DependencyInstaller* installer = nullptr;
    PyLibInstaller* py_installer = nullptr;
    ChoiceGuide* guide = nullptr;
    //密码对话框，由所有可能询问密码的窗体共享
    QInputDialog* pwdDialog = nullptr;

    //用户的配置选择
    Choice* _userChoice = nullptr;

public:
    Core(QApplication* app);
    ~Core();

private:
    //初始化函数
    //在启动所有窗体前，从config.json文件中读取配置
    void readConfig();
    //初始化密码对话框GUI
    inline void initPwdDialog();
    //初始化信号槽连接
    inline void initConnections();
private:
    //检查用户是否配置成功
    bool checkConfigured();
    //根据用户配置，检查脚本是否成功生成，若用户没有配置，直接返回true
    bool checkGenScript();
private:
    //关闭软件时写入配置
    void writeConfig();
public:
    //配置完成后，复制用户选择
    void copyUserChoice(const Choice* _userChoice);
     //返回是否有进程正在运行
    bool isProcessRunning() const;

private slots:
    //清理脚本
    void cleanScript();
    //根据用户选择，生成脚本
    void genScript();
    //性能仿真
    void simulatePerformance();
    //生成温度图
    void genHeatMap();
public slots:
    //终止当前任务
    void terminate();
private:
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
    //TODO : 报错
    void reportError(QString errMsg);
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

    //日志信号
    void logProgram(QString program,QString info);
    void log(QString info);
};

#endif // CORE_H
