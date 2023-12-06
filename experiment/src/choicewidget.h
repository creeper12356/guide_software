#ifndef USERCHOICEWIDGET_H
#define USERCHOICEWIDGET_H
#include "inc.h"
class Choice;

namespace Ui {
class ChoiceWidget;
}

class ChoiceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChoiceWidget(QWidget *parent = 0);
    ~ChoiceWidget();
public slots:
    //将用户选择更新到界面上
    void refreshUserChoice(const Choice *userChoice);
private:
    Ui::ChoiceWidget *ui;
};

#endif // USERCHOICEWIDGET_H
