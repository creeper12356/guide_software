#include "choiceguide.h"
#include "ui_choiceguide.h"

ChoiceGuide::ChoiceGuide(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ChoiceGuide)
{
    ui->setupUi(this);
    loadProgramChoices();
    int count = 0;
    for(auto key:programInfo->keys()){
        QPushButton* newButton = new QPushButton(key,this);
        ui->arch_grid->addWidget(newButton,0,count);
        ++count;
        connect(newButton,&QPushButton::clicked,this,&ChoiceGuide::ChoiceButtonClickSlot);
    }
}

ChoiceGuide::~ChoiceGuide()
{
    delete ui;
    delete programInfo;
}

void ChoiceGuide::showEvent(QShowEvent *event)
{
    ui->stacked_widget->setCurrentIndex(0);
    ui->prev_button->setEnabled(false);
    ui->next_button->setEnabled(false);
    ui->finish_button->setEnabled(false);
}

void ChoiceGuide::loadProgramChoices()
{
    QFile reader("./program_choices.json");
    reader.open(QIODevice::ReadOnly);
    programInfo = new QJsonObject(QJsonDocument::fromJson(reader.readAll()).object());
    reader.close();
}

void ChoiceGuide::on_next_button_clicked()
{
    ui->stacked_widget->setCurrentIndex(ui->stacked_widget->currentIndex() + 1);
}

void ChoiceGuide::on_stacked_widget_currentChanged(int index)
{
    qDebug() << "index: " << index;
    ui->prev_button->setEnabled(index != 0);
    ui->next_button->setEnabled(index != ui->stacked_widget->count() - 1);
    ui->finish_button->setEnabled(index == ui->stacked_widget->count() - 1);
}

void ChoiceGuide::ChoiceButtonClickSlot()
{
    qDebug() << "choice button clicked.";
    QPushButton* button = reinterpret_cast<QPushButton* > (sender());
    qDebug() << button->text();
    ui->next_button->setEnabled(true);
    int count = 0;
    if(!programInfo->contains(button->text())){
        qDebug() << "key not existed";
        return ;
    }
    QJsonValue arch = programInfo->operator [](button->text());
    if(!arch.isObject()){
        qDebug() << "not implemented yet.";
        return ;
    }
    for(auto key:arch.toObject().keys()){
        QPushButton* newButton = new QPushButton(key,this);
        ui->set_grid->addWidget(newButton,0,count);
        ++count;
        connect(newButton,&QPushButton::clicked,this,&ChoiceGuide::ChoiceButtonClickSlot);
    }
}

void ChoiceGuide::on_prev_button_clicked()
{
    ui->stacked_widget->setCurrentIndex(ui->stacked_widget->currentIndex() - 1);
}

void ChoiceGuide::on_cancel_button_clicked()
{
    this->close();
}

void ChoiceGuide::on_finish_button_clicked()
{
    //TODO
    this->close();
}
