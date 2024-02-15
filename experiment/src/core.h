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

    /*!
     * \brief 检查用户是否配置成功
     * \return 用户是否配置成功
     */
    bool checkConfigured();

    /*!
     * \brief 检查脚本是否成功生成
     * \return 如果用户未配置，返回true；如果用户已配置，返回所有的脚本是否成功生成。
     */
    bool checkGenScript();

public slots:
    //!清空配置
    void clearConfig();
    //!清空脚本
    void cleanScript();
    //!生成脚本
    void genScript();
    //!性能仿真
    void simulatePerformance();
    //!生成温度图
    void genHeatMap();
    //!探针函数
    void probe(QString program , qreal probeX , qreal probeY);
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
     * \brief 分割gem5输出的性能数据
     * \param program 基准程序名
     * \return 是否分割成功
     */
    bool splitGem5Output(const QString& program);

    /*!
     * \brief 将分割后的性能数据转换为xml文件
     * \param program 基准程序名
     */
    void genXml(const QString& program);

    /*!
     * \brief 运行McPAT模块
     * \param program 基准程序名
     */
    void runMcpat(const QString &program);

    /*!
     * \brief 将功耗数据转换为ptrace文件
     * \param program 基准程序名
     */
    void writePtrace(const QString& program);

    /*!
     * \brief 运行HotSpot模块，生成grid.steady文件
     * \param program 基准程序名
     */
    void runHotspot(const QString& program);

    /*!
     * \brief 调用脚本绘制温度图
     * \param program 基准程序名
     */
    void drawHeatMap(const QString& program);

    /*!
     * \brief 写入日志
     * \param info 写入的信息
     * \sa logConsoleProgram
     */
    void logConsole(const QString& info);

    /*!
     * \brief 格式化写入日志
     *
     * 等价于写入：程序"{program}":{info}
     * \param program 基准程序名
     * \param info 写入信息
     * \sa logConsole
     */
    void logConsoleProgram(const QString& program, const QString& info);

signals:

    void probeResult(qreal temperature , qreal probeX, qreal probeY);

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
    //缓冲区,记录上一次readAll操作读到的进程输出

    MainPage* mMainPage = nullptr;
    AptInstaller* mAptInstaller = nullptr;
    PyLibInstaller* mPyLibInstaller = nullptr;
};

#endif // CORE_H
