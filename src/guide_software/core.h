#pragma once
#ifndef CORE_H
#define CORE_H
#include "inc.h"
class MainPage;
class DependencyInstaller;
class PyLibInstaller;
class ChoiceGuide;
struct Choice;

class Core:public QObject
{
    Q_OBJECT
private:
    QJsonObject* config = nullptr;
    QApplication* app = nullptr;
    QEventLoop* eventLoop = nullptr;
    QProcess* proc = nullptr;

    MainPage* mainPage = nullptr;
    DependencyInstaller* installer = nullptr;
    PyLibInstaller* py_installer = nullptr;
    ChoiceGuide* guide = nullptr;
    //由所有可能询问密码的窗体共享
    QInputDialog* pwdDialog = nullptr;
    //指向ChoiceGuide::userChoice的副本
    Choice* userChoice = nullptr;

public:
    Core(QApplication* app);
    ~Core();
private:
    //初始化函数
    //在启动所有窗体前，从config.json文件中读取配置
    void configure();
    //初始化密码对话框GUI
    inline void initPwdDialog();
    //初始化信号槽连接
    inline void initConnections();
private:
    //检查用户是否配置成功
    bool checkConfigured();
    //根据用户配置，检查脚本是否成功生成，若用户没有配置，直接返回true
    bool checkScriptGenerated();
public:
    //配置完成后，复制用户选择
    void copyUserChoice(Choice* userChoice);

private slots:
    //清理脚本
    void cleanScript();
    //根据用户选择，生成脚本
    void generateScript();
    //性能仿真
    void performanceSimulate();

    //TODO : 报错
    void reportError(QString errMsg);
signals:
    //清理脚本成功的信号
    void scriptCleaned();
    //脚本成功生成的信号
    void scriptGenerated();
};

#endif // CORE_H
