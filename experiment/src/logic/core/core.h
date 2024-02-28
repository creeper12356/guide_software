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

public slots:
    //!终止
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
     *
     * 结束所有进程并退出。
     */
    void forceQuit();

public:
    /*!
     * \brief 写入日志
     * \param info 写入的信息
     * \sa logProgram
     */
    void log(const QString& info);

    /*!
     * \brief 格式化写入日志
     *
     * 等价于写入：程序"{program}":{info}
     * \param program 基准程序名
     * \param info 写入信息
     * \sa log
     */
    void logProgram(const QString& program, const QString& info);

signals:
    //!耗时任务开始
    void longTaskStarted();
    //!耗时任务完成
    void longTaskFinished();

    void warning(QString info);
    void critical(QString info);

    //!询问用户是否退出
    void askQuit();

    //!退出信号，发出该信号将无条件终止软件
    void quit();

private:

    QApplication* mApp = nullptr;
    AppModel* mAppModel = nullptr;

    //调用外部程序的进程
    //pub_proc为公有进程，将输出打印到终端，更新cache
    //pri_proc为私有进程，不打印输出，只更新cache
    TaskProcess* mPubProc = nullptr;
    TaskProcess* mPriProc = nullptr;

    MainPage* mMainPage = nullptr;
    AptInstaller* mAptInstaller = nullptr;
    PyLibInstaller* mPyLibInstaller = nullptr;

    PerformanceCore* mPerformanceCore;
    HeatMapCore* mHeatMapCore;
};

#endif // CORE_H
