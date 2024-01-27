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
private:
    QToolBar* toolBar = nullptr;

    ConsoleDock* consoleDock = nullptr;
    QDockWidget* logDock = nullptr;

    ImageDisplay* heatMap = nullptr;
    ChoiceWidget* choiceWidget = nullptr;

    ChoiceGuide* mGuide;
public:
    MainPage();
    ~MainPage();
public:
    //getters
    Ui::MainPage* getUi(){return ui;}
    ConsoleDock* getConsoleDock();
    ChoiceWidget* getChoiceWidget();
    QTextBrowser* getLogBrowser();
private:
    inline void initToolBar() ;
    inline void initDockWidgets() ;
private slots:
    void configureTriggered();
    void on_action_exit_triggered();
    void maximizeTriggered();
    void on_action_show_heatmap_triggered();

    void aboutTriggererd();
    void aboutqtTriggered();

public slots:
    void scriptCleanedSlot();
    void scriptGeneratedSlot();
    void performanceSimulationFinishedSlot();

    void longTaskStartedSlot();
    void longTaskFinishedSlot();

public:
    void logConsole(const QString& info);
    void logConsoleProgram(const QString &program, const QString &info);

protected:
    void closeEvent(QCloseEvent *event) override;
private:
    Ui::MainPage *ui;
signals:
    void configureFinished(const Choice* userChoice);
};

#endif // MAINPAGE_H
