#pragma once
#ifndef MAINPAGE_H
#define MAINPAGE_H
#include "inc.h"

namespace Ui {
class MainPage;
}
class Core;
class MainPage : public QMainWindow
{
    Q_OBJECT
private:
    Core* core = nullptr;
public:
    MainPage(Core* core,QWidget *parent = nullptr);
    ~MainPage();
public:
    //getters
    Ui::MainPage* getUi(){return ui;}
private slots:
    void on_action_about_triggered();
    void on_action_exit_triggered();
public slots:
    void scriptCleanedSlot();
    void scriptGeneratedSlot();
protected:
    void closeEvent(QCloseEvent *event) override;
private:
    Ui::MainPage *ui;
signals:
    void closed();
};

#endif // MAINPAGE_H
