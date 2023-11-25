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
    void on_exit_button_clicked();
public slots:
    //在界面上显示生成脚本
    void displayWritingScript(bool state);
protected:
    void keyPressEvent(QKeyEvent* event) override;
    void closeEvent(QCloseEvent *event) override;
private:
    Ui::MainPage *ui;
signals:
    void closed();
};

#endif // MAINPAGE_H
