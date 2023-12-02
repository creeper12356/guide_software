#pragma once
#ifndef DEPENDENCYINSTALLER_H
#define DEPENDENCYINSTALLER_H
#include "inc.h"

namespace Ui {
class DependencyInstaller;
}
//依赖安装程序的抽象基类
class DependencyInstaller : public QDialog
{
    Q_OBJECT
protected:
    QString updateCmd;
    QString getNotInstalledCmd;
    QString installCmd;
    //安装是否需要密码
    bool isPasswdNeeded = true;
private:
    Core* core = nullptr;
    //指向Core的eventLoop成员，用于处理GUI事件，防止GUI冻结
    QEventLoop*& eventLoop;
    //密码输入对话框
    QInputDialog*& pwdDialog;
    //运行bash的进程
    QProcess* proc = nullptr;
    //未安装依赖列表
    QStringList pkgList;

    bool isAccepted = false;

public:
    DependencyInstaller(Core* core,QEventLoop*& eventLoop,QInputDialog*& pwdDialog,QWidget *parent = nullptr);
    virtual ~DependencyInstaller();
    //getters
    Ui::DependencyInstaller* getUi(){return ui;}
protected:
    //派生类必须实现，初始化命令
    virtual void initCmds() = 0;
private:
    //使用命令检查依赖，返回是否已经全部安装
    bool checkDependencies();
    //使用密码pwd安装依赖
    void installDependencies(QString pwd);
public:
    //检查并安装依赖，供外部类调用，返回最终所有依赖是否成功安装
    bool checkAndInstall();
public slots:
    //GUI函数
    void switchCheckGUI();
    void switchInstallGUI();
    void showUnmetDependencies(const QStringList& list);
private slots:
    void setAccepted();
private:
    Ui::DependencyInstaller *ui;
signals:
    //错误信号
    void error(QString errMsg);
    //发送给界面的信号，使界面实时显示安装进度，value范围[0,100]
    void installProcess(int value);
};
//基于apt的安装程序
class AptInstaller: public DependencyInstaller
{
public:
    AptInstaller(Core* core,
                 QEventLoop*& eventLoop,
                 QInputDialog*& pwdDialog,
                 QWidget* parent = nullptr);
protected:
    void initCmds() override;
};
//基于pip的python包安装程序
class PyLibInstaller: public DependencyInstaller{
public:
    PyLibInstaller(Core* core,
                   QEventLoop*& eventLoop,
                   QInputDialog*& pwdDialog,
                   QWidget *parent = nullptr);
protected:
    void initCmds() override;
};


#endif // DEPENDENCYINSTALLER_H
