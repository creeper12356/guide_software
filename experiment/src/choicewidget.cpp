#include "choicewidget.h"
#include "ui_choicewidget.h"
ChoiceWidget::ChoiceWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChoiceWidget)
{
    ui->setupUi(this);
    connect(ui->prog_list,&QListWidget::currentTextChanged,
            this,&ChoiceWidget::currentTextChanged);
}

ChoiceWidget::~ChoiceWidget()
{
    delete ui;
}

void ChoiceWidget::refreshUserChoice(const Choice *userChoice)
{
    ui->prog_list->clear();
    ui->prog_list->addItems(userChoice->programs);
    ui->test_label->setText(userChoice->test);
    ui->thread_num_label->setText(QString::number(userChoice->threadNum));
}
