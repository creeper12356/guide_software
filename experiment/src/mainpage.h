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

    void initToolBar() ;
    void initDockWidgets() ;
    void restoreStateAndGeometry();
    void saveStateAndGeometry();

public:
    ConsoleDock* consoleDock();
    ChoiceWidget* choiceWidget();
    QTextBrowser* logBrowser();

public:
    void logConsole(const QString& info);
    void logConsoleProgram(const QString &program, const QString &info);
protected:
    void closeEvent(QCloseEvent *event);
public slots:
    void updateUserChoice(const Choice* userChoice);
    void cleanScriptFinishedSlot();
    void genScriptFinishedSlot();
    void genScriptFailedSlot(QString warningInfo);
    void performanceSimulationFinishedSlot();
    void performanceSimulationFailedSlot(QString warningInfo);

    void longTaskStartedSlot();
    void longTaskFinishedSlot();

    void askQuitSlot();

private:
    void warning(const QString& info);
    void critical(const QString& info);

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
    //向core请求退出软件的信号
    void quit();
    void forceQuit();

private:
    QToolBar* mToolBar = nullptr;

    ConsoleDock* mConsoleDock = nullptr;
    QDockWidget* mLogDock = nullptr;

    ImageDisplay* mHeatMapDisplay = nullptr;
    ChoiceWidget* mChoiceWidget = nullptr;

    ChoiceGuide* mGuide;
private:
    Ui::MainPage *ui;
};

#endif // MAINPAGE_H
