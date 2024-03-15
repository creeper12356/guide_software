#ifndef MAINPAGE_H
#define MAINPAGE_H
#include "inc.h"

namespace Ui {
class MainPage;
}

/*!
 * \brief 主页面类
 * \details 软件的主页面类，托管软件所有的GUI事件。
 *          1. 松耦合与Core通信。MainPage的函数和信号接口对Core可见，Core对MainPage不可见，二者通过信号与槽机制通信。
 *          2. 因为Core只能调用MainPage提供的函数和信号接口，
 *          因此所有的GUI事件都由MainPage托管，所有的GUI组件都为MainPage的成员。
 *          3. 添加新的界面组件，不要直接写在MainPage中，而是应该先抽象成组件，
 *          作为MainPage的数据成员，并在MainPage中定义相关的接口。
 * \sa Core
 */
class MainPage : public QMainWindow
{
    Q_OBJECT
public:
    MainPage();
    ~MainPage();

    // 初始化函数
    void initDockWidgets() ;
    void initSplitter();
    void initConnections();

    /*!
     * \brief 恢复上次正常关闭的窗口状态和布局
     * \sa saveStateAndGeometry
     */
    void restoreStateAndGeometry();

    /*!
     * \brief 保存窗口状态和布局
     * \sa restoreStateAndGeometry
     */
    void saveStateAndGeometry();



public slots:

    /*!
     * \brief 更新用户选择在界面上
     * \details 将用户的选择显示在配置窗口的列表中
     * \param userChoice 用户选择
     */
    void updateUserChoice(const Choice* userChoice);

    /*!
     * \brief 显示探针的结果
     * \details 将探针对应的温度、坐标显示到探针窗口
     */
    void displayProbeResult(qreal temperature,qreal probeX,qreal probeY);


    /*!
     * \brief Core::longTaskStarted的槽函数，GUI页面做出调整。
     * \sa Core::longTaskStarted
     */
    void longTaskStartedSlot();

    /*!
     * \brief Core::longTaskFinished的槽函数，GUI页面撤销调整。
     * \sa Core::longTaskFinished
     */
    void longTaskFinishedSlot();




    /*!
     * \brief 弹出对话框询问是否强制退出
     */
    void askQuitSlot();

signals:
    /*!
     * \brief 退出信号
     * \details 发送给Core，请求退出软件，
     *          如果存在正在运行的进程，Core将发送Core::askQuit信号，触发askQuitSlot，询问是否强制退出。
     *
     * \sa Core::quit
     * \sa Core::askQuit
     * \sa askQuitSlot
     * \sa forceQuit
     */
    void quit();

    /*!
     * \brief 强制退出信号
     * \details 发送给Core，触发Core::forceQuit，无条件终止软件。
     *
     * \sa Core::forceQuit
     * \sa quit
     */
    void forceQuit();


public slots:

    /*!
     * \brief 终端窗口追加标准输入
     * \details 在终端窗口显示标准输入的内容，e.g. ~/Documents$ ls -al .
     * \param dir 命令执行的文件目录
     * \param info 标准输入（命令）的内容
     */
    void consoleAppendStdin(QString dir , QString info);

    /*!
     * \brief 终端窗口追加标准输出
     * \param info 标准输出的内容
     */
    void consoleAppendStdout(QString info);

    /*!
     * \brief 终端窗口追加标准错误输出
     * \param info 标准错误输出的内容
     */
    void consoleAppendStderr(QString info);



public:
    /*!
     * \brief 写入日志
     * \details 将内容info写入日志窗口, 如果info为空将清空日志。
     * \param info 写入的信息
     * \sa logProgram
     */
    void log(const QString& info);

    /*!
     * \brief 格式化写入日志
     * \details 将内容 程序"{program}":{info} 写入日志窗口
     * \param program 基准程序名
     * \param info 写入信息
     * \sa log
     */
    void logProgram(const QString& program, const QString& info);




public slots:
    /*!
     * \brief 显示警告的对话框
     * \param info 警告内容
     */
    void warning(QString info);
    /*!
     * \brief 显示错误的对话框
     * \param info 错误内容
     */
    void critical(QString info);




    // {Action}Triggered 函数
    // 用户触发QAction后调用的槽函数，
    // 槽函数会发送对应的信号与Core进行通信。
private slots:
    void configureTriggered();
    void simulatePerformanceTriggered();
    void genHeatMapTriggered();
    void aboutTriggererd();
    void aboutqtTriggered();

signals:
    // {Action} 信号
    // 用户在触发QAction后，可以直接触发{Action}信号，
    // 也可以经过{Action}Triggered槽函数包装后，间接触发{Action}信号。
    void configureFinished(const Choice* userChoice);
    void clearConfig();
    void cleanScript();
    void genScript();
    void simulatePerformance();
    void genHeatMap();
    void terminate();
    void probe(QString program ,qreal x, qreal y);




protected:
    void closeEvent(QCloseEvent *event);
private:
    //!终端窗口
    ConsoleDock* mConsoleDock;
    //!日志窗口
    LogDock* mLogDock;


    ChoiceGuide* mGuide;

    QSplitter* mMainSplitter;
    QSplitter* mLeftSplitter;
private:
    Ui::MainPage *ui;
};

#endif // MAINPAGE_H
