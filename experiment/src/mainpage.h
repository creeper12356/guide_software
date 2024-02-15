#ifndef MAINPAGE_H
#define MAINPAGE_H
#include "inc.h"

class Choice;
namespace Ui {
class MainPage;
}
class MainPage : public QMainWindow
{
    Q_OBJECT
public:
    MainPage();
    ~MainPage();

    void initDockWidgets() ;
    void initSplitter();
    void initConnections();
    void restoreStateAndGeometry();
    void saveStateAndGeometry();

public:
    ConsoleDock* consoleDock();
    ChoiceWidget* choiceWidget();
public slots:

    /*!
     * \brief 更新用户选择在界面上
     * \param userChoice 用户选择
     */
    void updateUserChoice(const Choice* userChoice);

    void cleanScriptFinishedSlot();
    void genScriptFinishedSlot();
    void genScriptFailedSlot(QString warningInfo);
    void performanceSimulationFinishedSlot();
    void performanceSimulationFailedSlot(QString warningInfo);
    void genHeatMapFinishedSlot();
    void displayProbeResult(qreal temperature,qreal probeX,qreal probeY);

    void consoleAppendStdin(QString dir , QString info);
    void consoleAppendStdout(QString info);
    void consoleAppendStderr(QString info);

    void longTaskStartedSlot();
    void longTaskFinishedSlot();

    void askQuitSlot();

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
private slots:
    void configureTriggered();
    void simulatePerformanceTriggered();
    void genHeatMapTriggered();
    void maximizeTriggered();
    void aboutTriggererd();
    void aboutqtTriggered();

signals:
    //用户配置完成的信号，发送给core
    void configureFinished(const Choice* userChoice);
    void clearConfig();
    void cleanScript();
    void genScript();
    void simulatePerformance();
    void genHeatMap();
    void terminate();
    //探针信号
    void probe(QString program ,qreal x, qreal y);
    //向core请求退出软件的信号
    void quit();
    void forceQuit();

public:
    void logConsole(const QString& info);
    void logConsoleProgram(const QString &program, const QString &info);
protected:
    void closeEvent(QCloseEvent *event);


private:
    ConsoleDock* mConsoleDock;
    LogDock* mLogDock;
    ChoiceGuide* mGuide;

    QSplitter* mMainSplitter;
    QSplitter* mLeftSplitter;
private:
    Ui::MainPage *ui;
};

#endif // MAINPAGE_H
