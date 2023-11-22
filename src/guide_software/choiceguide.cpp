#include "choiceguide.h"
#include "ui_choiceguide.h"

ChoiceGuide::ChoiceGuide(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ChoiceGuide)
{
    ui->setupUi(this);
    loadProgramChoices();
    int count = 0;
    for(auto key:programInfo.keys()){
        QPushButton* newButton = new QPushButton(key,this);
        ui->arch_grid->addWidget(newButton,0,count);
        ++count;
        connect(newButton,&QPushButton::clicked,this,[newButton,this](){
            this->architectChosenSlot(newButton->text());
            qDebug() << newButton->text();
        });
    }
}

ChoiceGuide::~ChoiceGuide()
{
    delete ui;
}

void ChoiceGuide::showEvent(QShowEvent *event)
{
    ui->stacked_widget->setCurrentIndex(0);
    ui->prev_button->setEnabled(false);
    ui->next_button->setEnabled(true);
    ui->finish_button->setEnabled(false);
    programChoice.architecture = "";
    programChoice.set = "";
    programChoice.programs.clear();
}

void ChoiceGuide::loadProgramChoices()
{
    QFile reader("./program_choices.json");
    reader.open(QIODevice::ReadOnly);
    programInfo = QJsonDocument::fromJson(reader.readAll()).object();
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
    if(index == ui->stacked_widget->count() - 1){
        //reaches last page
        if(!programChoice.architecture.isEmpty()
                && !programChoice.set.isEmpty()
                && !programChoice.programs.isEmpty()){
            ui->finish_button->setEnabled(true);
        }
    }
    else{
        ui->finish_button->setDisabled(true);
    }
}


void ChoiceGuide::architectChosenSlot(QString name)
{
    //backend
    programChoice.architecture = name;
    programChoice.set = "";
    programChoice.programs.clear();

    //frontend
    while(ui->set_grid->count()){
        delete ui->set_grid->takeAt(0)->widget();
    }
    ui->prog_list->clear();

    if(programInfo[name].isBool()){
        //not implemented
        return ;
    }
    auto sets = programInfo[name].toObject();
    for(auto key:sets.keys()){
        QPushButton *newButton = new QPushButton(key,this);
        ui->set_grid->addWidget(newButton);
        connect(newButton,&QPushButton::clicked,this,[this,newButton](){
           this->setChosenSlot(newButton->text());
        });
    }
}

void ChoiceGuide::setChosenSlot(QString name)
{
    //backend
    programChoice.set = name;
    programChoice.programs.clear();
    //frontend
    ui->prog_list->clear();
    if(programInfo[programChoice.architecture].toObject()[name].isBool()){
        //not implemented
        return ;
    }
    auto programs = programInfo[programChoice.architecture].toObject()[name].toArray();

    for(auto program:programs){
        ui->prog_list->addItem(program.toString());
    }
}

void ChoiceGuide::programChosenSlot(QString name)
{
    //backend
    if(programChoice.programs.contains(name)){
        return ;
    }
    programChoice.programs.push_back(name);
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
