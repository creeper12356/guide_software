#ifndef DEPENDENCYINSTALLER_H
#define DEPENDENCYINSTALLER_H
#include "inc.h"

namespace Ui {
class DependencyInstaller;
}
/*!
 * \brief 前置软件安装对话框抽象类
 *
 * Inherited by AptInstaller , PyLibInstaller
 */
class DependencyInstaller : public QDialog
{
    Q_OBJECT

public:
    DependencyInstaller(Core* c);
    virtual ~DependencyInstaller();

    /*!
     * \brief 检查并安装依赖
     * \return 调用该函数后依赖是否存在
     */
    bool checkAndInstall();

protected:

    /*!
     * \brief 更新安装源的命令
     *
     * 若该字段非空，updateCommand()为更新安装源的命令，在每次安装前执行，
     * 例如sudo apt update；
     * 若该字段为空，表示安装前无需更新。
     */
    virtual QString updateCommand() = 0;

    /*!
     * \brief 获取未安装依赖列表的命令
     */
    virtual QString getNotInstalledCommand() = 0;

    /*!
     * \brief 安装命令
     *
     * 若isSuperUser() 为true，安装命令为installCommand().arg(password,dependency)；
     * 若isSuperUser() 为false，安装命令为installCommand().arg(dependency)。
     */
    virtual QString installCommand() = 0;

    /*!
     * \brief 安装是否需要密码
     */
    virtual bool isSuperUser() = 0;

    /*!
     * \brief 检查模式下提示的信息
     */
    virtual QString hintText() = 0;

protected slots:
    void switchCheckGUI();
    void switchInstallGUI();
    void showUnmetDependencies(const QStringList& list);
private:
    //使用命令检查依赖，返回是否已经全部安装
    bool checkDependencies();
    //使用密码pwd安装依赖
    void installDependencies(QString pwd);

private slots:
    void setAccepted();
private:
    Ui::DependencyInstaller *ui;
signals:
    //发送给界面的信号，使界面实时显示安装进度，value范围[0,100]
    void installProcess(int value);
private:
    Core* core = nullptr;
    //指向Core的eventLoop成员，用于处理GUI事件，防止GUI冻结
    QEventLoop* mEventLoop;
    //密码输入对话框
    PasswordDialog* mPasswordDialog = nullptr;
    //运行bash的进程
    QProcess* mProcess = nullptr;
    //未安装依赖列表
    QStringList mDependencyList;

    bool mAccepted = false;
};

//基于apt的安装程序
class AptInstaller: public DependencyInstaller
{
public:
    AptInstaller(Core* core);
protected:
    QString updateCommand() override;
    QString getNotInstalledCommand() override;
    QString installCommand() override;
    bool isSuperUser() override;
    QString hintText() override;
};

//基于pip的python包安装程序
class PyLibInstaller: public DependencyInstaller{
public:
    PyLibInstaller(Core* core);
protected:
    QString updateCommand() override;
    QString getNotInstalledCommand() override;
    QString installCommand() override;
    bool isSuperUser() override;
    QString hintText() override;
};


#endif // DEPENDENCYINSTALLER_H
