#ifndef MAINPAGE_H
#define MAINPAGE_H

#include <QMainWindow>

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
    explicit MainPage(Core* core,QWidget *parent = nullptr);
    ~MainPage();

private slots:
    void on_exit_button_clicked();

private:
    Ui::MainPage *ui;
};

#endif // MAINPAGE_H
