#pragma once
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
    Core* core = nullptr;

    QToolBar* toolBar = nullptr;

    QDockWidget* terminalDock = nullptr;
//    QDockWidget* choiceDock = nullptr;
    QDockWidget* logDock = nullptr;

    ImageDisplay* heatMap = nullptr;
    ChoiceWidget* choiceWidget = nullptr;
public:
    MainPage(Core* core,QWidget *parent = nullptr);
    ~MainPage();
public:
    //getters
    Ui::MainPage* getUi(){return ui;}
    QTextBrowser* getTerminalReflect();
    ChoiceWidget* getChoiceWidget();
    QTextBrowser* getLogBrowser();
private:
    inline void initToolBar() ;
    inline void initDockWidgets() ;
private slots:
    void on_action_about_triggered();
    void on_action_exit_triggered();
    void on_action_maximize_triggered();

    void on_action_show_heatmap_triggered();

public slots:
    void scriptCleanedSlot();
    void scriptGeneratedSlot();
    void performanceSimulationFinishedSlot();

    void longTaskStartedSlot();
    void longTaskFinishedSlot();

    void refreshLog(QString info);
    void refreshLogProgram(QString program,QString info);
protected:
    void closeEvent(QCloseEvent *event) override;
private:
    Ui::MainPage *ui;
signals:
    void closed();
};

#endif // MAINPAGE_H
