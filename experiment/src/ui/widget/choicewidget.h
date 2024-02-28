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
    QSize sizeHint() const override;
public slots:
    //将用户选择更新到界面上
    void refreshUserChoice(const Choice *userChoice);
private:
    Ui::ChoiceWidget *ui;
signals:
    void currentTextChanged(const QString& currentText);
};

#endif // USERCHOICEWIDGET_H
