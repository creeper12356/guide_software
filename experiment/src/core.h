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
    //pub_proc : 进程输出会显示出来
    //pri_proc : 进程输出不显示
    QProcess* pub_proc = nullptr;
    QProcess* pri_proc = nullptr;

    MainPage* mainPage = nullptr;
    DependencyInstaller* installer = nullptr;
    PyLibInstaller* py_installer = nullptr;
    ChoiceGuide* guide = nullptr;
    //由所有可能询问密码的窗体共享
    QInputDialog* pwdDialog = nullptr;
    //指向ChoiceGuide::_userChoice的副本
    Choice* _userChoice = nullptr;
    //记录上一次readAll操作读到的进程输出
    QByteArray cache;

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

private slots:
    //清理脚本
    void cleanScript();
    //根据用户选择，生成脚本
    void genScript();
    //性能仿真
    void simulatePerformance();
    //生成温度图
    void genTempGraph();
private:
    //处理gem5输出，program对应的性能数据, temporarily useless?
    bool splitGem5Output(const QString& program);
    //运行mcpat模块，处理program对应的xml文件
    void runMcpat(const QString &program);
    //
    //TODO : 报错
    void reportError(QString errMsg);
signals:
    //finished signal
    //清理脚本成功的信号
    void cleanScriptFinished();
    //脚本成功生成的信号
    void genScriptFinished();
    //性能仿真运行成功信号
    void simulatePerformanceFinished();
};

#endif // CORE_H
