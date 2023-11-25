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

private:
    //在启动所有窗体前，从config.json文件中读取配置
    void configure();
public:
    Core(QApplication* app);
    ~Core();
private:
    //初始化函数
    //初始化密码对话框GUI
    inline void initPwdDialog();
    /* init signals and slots connection */
    //初始化信号槽连接
    inline void initConnections();
public:
    //复制用户选择
    void copyUserChoice(Choice* userChoice);
private slots:
    void reportError(QString errMsg);
    //根据用户选择，生成脚本
    void writeScripts();
signals:
    //发送给前端的信号，true表示生成成功，false表示未生成
    void scriptsWriteState(bool state);
};

#endif // CORE_H
