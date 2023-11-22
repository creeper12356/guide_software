#pragma once
#ifndef CHOICEGUIDE_H
#define CHOICEGUIDE_H
#include "inc.h"

struct ProgramChoice{
    QString architecture = "";
    QString set = "";
    QStringList programs;
};

namespace Ui {
class ChoiceGuide;
}

class ChoiceGuide : public QMainWindow
{
    Q_OBJECT
private:
    /*
     * loaded from file ./program_choices.json
     * stores info of program choice branches
     */
    QJsonObject programInfo;
    ProgramChoice programChoice;
public:
    explicit ChoiceGuide(QWidget *parent = nullptr);
    ~ChoiceGuide();
protected:
    void showEvent(QShowEvent *event) override;
private:
    void loadProgramChoices();
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

private:
    Ui::ChoiceGuide *ui;
};

#endif // CHOICEGUIDE_H
