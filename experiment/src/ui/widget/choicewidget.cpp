#include "choicewidget.h"
#include "ui_choicewidget.h"

#include "data/utils/choice.h"

ChoiceWidget::ChoiceWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChoiceWidget)
{
    ui->setupUi(this);
    this->layout()->setAlignment(Qt::AlignTop);
    connect(ui->prog_list,&QListWidget::currentTextChanged,
            this,&ChoiceWidget::currentTextChanged);
}

ChoiceWidget::~ChoiceWidget()
{
    delete ui;
}

QSize ChoiceWidget::sizeHint() const
{
    return this->minimumSize();
}

void ChoiceWidget::refreshUserChoice(const Choice *userChoice)
{
    if(userChoice->isConfigured()){
        ui->hint_label->hide();
        ui->detail_widget->show();
        ui->prog_list->clear();
        ui->prog_list->addItems(userChoice->programs);
        ui->test_label->setText(userChoice->test);
        ui->thread_num_label->setText(QString::number(userChoice->threadNum));

    }
    else{
        ui->hint_label->show();
        ui->detail_widget->hide();
    }
}
