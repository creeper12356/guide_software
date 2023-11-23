#pragma once
#ifndef CHOICEGUIDE_H
#define CHOICEGUIDE_H
#include "inc.h"

//用户选择的结构体
struct Choice{
    QString architecture = "";
    QString set = "";
    QStringList programs;
    QString test = "";
    int threadNum = 0;
};

namespace Ui {
class ChoiceGuide;
}

class ChoiceGuide : public QMainWindow
{
    Q_OBJECT
private:
    const QStringList infos =
    {
        "请选择架构",
        "请选择程序集", "请选择程序", "请选择测试集，并输入线程数"
    };
private:
    /*
     * loaded from file ./program_choices.json
     * stores info of program choice branches
     */
    //使用QJsonObject表示的所有程序选择分支信息
    QJsonObject programInfo;
    //使用QJsonObject表示的测试集列表，测试集名称为所引
    QJsonObject testInfo;
    Choice userChoice;
public:
    explicit ChoiceGuide(QWidget *parent = nullptr);
    ~ChoiceGuide();
protected:
    void showEvent(QShowEvent *event) override;
private:
    //从json文件中加载选择分支信息
    void loadChoices();
private slots:
    /* jump back and force */
    void on_next_button_clicked();
    void on_prev_button_clicked();
    /* cancel and finish */
    void on_cancel_button_clicked();
    void on_finish_button_clicked();
    /* update states of buttons ans page index changes */
    void on_stacked_widget_currentChanged(int index);

    void architectChosenSlot(QString name);
    void setChosenSlot(QString name);
    void programChosenSlot(QString name);
    void testChosenSlot(QString name);
    void threadNumSetSlot(int threadNum);
    void refreshFinishState();

public:
private:
    Ui::ChoiceGuide *ui;
};

#endif // CHOICEGUIDE_H
