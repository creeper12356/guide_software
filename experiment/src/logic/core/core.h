#ifndef CORE_H
#define CORE_H
#include "inc.h"

/*!
 * \brief 核心类
 * \details Core类为软件的控制器，它的主要功能是集成软件各个模块，
 *          使用信号与槽机制将图形化界面、数据模型、子功能核心模块连接在一起。
 *          P.S: 具体的功能函数不要写在Core中，请写在SubCore的派生类中，然后在Core中添加对应派生类的成员。
 *          e.g: 具体的功能：runMcPat; SubCore的派生类： HeatMapCore; 派生类成员： mHeatMapCore。
 *
 * \sa SubCore
 */
class Core: public QObject
{
    Q_OBJECT

public:

    /*!
     * \brief Core构造函数
     * \param app 客户端指定的QApplication指针
     */
    Core(QApplication* app);

    ~Core();

    /*!
     * \brief 初始化信号槽连接
     * \details 初始化客户端的所有信号与槽连接，在构造函数中，所有成员内存分配完成之后调用。
     */
    void initConnections();



public slots:

    /*!
     * \brief 杀死公有进程、私有进程和它们的子进程（仅Linux可用）
     * \details 该函数调用killChildProcess杀死子进程
     * \sa killChildProcesses
     */
    void terminate();

    /*!
     * \brief 检查是否可以退出，并发送信号给界面
     *
     * 检查是否存在未结束的进程。如果不存在，直接退出；如果存在，发送askQuit信号。
     * \sa askQuit
     */
    void checkQuit();

    /*!
     * \brief 强制退出
     * \details 杀死所有正在进行的进程，发出quit信号退出软件。
     * \sa quit
     * 结束所有进程并退出。
     */
    void forceQuit();
signals:

    /*!
     * \brief 询问用户退出信号
     * \details 该信号连接到MainPage，将触发对话框，询问用户是否终止软件。
     */
    void askQuit();

    /*!
     * \brief 退出信号
     * \details 该信号连接到mApp， 将无条件终止软件运行。
     */
    void quit();

private:
    /*!
     * \brief 杀死进程的所有子进程（仅Linux可用）
     * \param pid 进程id
     * \return 若子进程存在且所有子进程被杀死，返回true;其他情况下返回false.
     */
    bool killChildProcesses(Q_PID pid);



public:
    /*!
     * \brief 写入日志
     * \details 将内容info写入日志窗口，如果信息为空将清空日志。
     * \param info 写入的信息
     * \sa logProgram
     */
    void log(const QString& info);

    /*!
     * \brief 格式化写入日志
     * \details 将内容 程序"{program}":{info} 写入日志窗口。
     * \param program 基准程序名
     * \param info 写入信息
     * \sa log
     */
    void logProgram(const QString& program, const QString& info);



signals:
    /*!
     * \brief 警告信号
     * \details 触发警告对话框，内容为info
     * \param info
     */
    void warning(QString info);

    /*!
     * \brief 错误信号
     * \details 触发错误对话框，内容为info
     * \param info
     */
    void critical(QString info);


signals:
    /*!
     * \brief 耗时任务开始信号
     * \details 该信号与MainPage连接，当耗时任务开始时发出此信号，GUI页面做出调整。
     * \sa MainPage::longTaskStartedSlot
     */
    void longTaskStarted();

    /*!
     * \brief 耗时任务结束信号
     * \details 该信号与MainPage连接，当耗时任务结束时发出此信号，GUI页面撤销调整。
     * \sa MainPage::longTaskFinishedSlot
     */
    void longTaskFinished();



private:
    // 基本
    //!客户端连接的QApplication类指针
    QApplication* mApp = nullptr;
    //!数据模型，所有文件读写、维护数据的托管类
    AppModel* mAppModel = nullptr;
    //!主页面，所有GUI的托管类
    MainPage* mMainPage = nullptr;

    // 依赖安装相关
    //!基于apt的安装依赖窗口
    AptInstaller* mAptInstaller = nullptr;
    //!基于pip的安装依赖窗口
    PyLibInstaller* mPyLibInstaller = nullptr;

    // 进程相关
    //!调用外部程序的公有进程，进程的输入与输出将打印到终端窗口
    TaskProcess* mPubProc = nullptr;
    //!调用外部程序的私有进程，进程的输入与输出不会打印到终端窗口
    TaskProcess* mPriProc = nullptr;

    // 子功能核心模块
    PerformanceCore* mPerformanceCore;
    HeatMapCore* mHeatMapCore;
};

#endif // CORE_H
