#include "choiceguide.h"
#include "ui_choiceguide.h"

ChoiceGuide::ChoiceGuide(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ChoiceGuide)
{
    ui->setupUi(this);
    loadChoices();
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
    for(auto key:testInfo.keys()){
        ui->test_list->addItem(key);
    }
    connect(ui->prog_list,&QListWidget::itemClicked,this,[this](QListWidgetItem* item){
        qDebug() << item->text();
        this->programChosenSlot(item->text());
    });
    connect(ui->test_list,&QListWidget::itemClicked,this,[this](QListWidgetItem* item){
        qDebug() << item->text();
        this->testChosenSlot(item->text());
    });
    connect(ui->thread_num_box,SIGNAL(valueChanged(int)),this,SLOT(threadNumSetSlot(int)));
}

ChoiceGuide::~ChoiceGuide()
{
    delete ui;
}

void ChoiceGuide::showEvent(QShowEvent *)
{
    ui->stacked_widget->setCurrentIndex(0);
    ui->info_label->setText(infos[0]);
    //init button state
    ui->prev_button->setEnabled(false);
    ui->next_button->setEnabled(true);
    ui->finish_button->setEnabled(false);
    //init backend state
    userChoice.architecture = "";
    userChoice.set = "";
    userChoice.programs.clear();
    userChoice.test = "";
    userChoice.threadNum = 0;
    //init frontend state
    while(ui->set_grid->count()){
        delete ui->set_grid->takeAt(0)->widget();
    }
    ui->prog_list->clear();
    ui->thread_num_box->setValue(0);
}

void ChoiceGuide::loadChoices()
{
    QFile reader("./program_choices.json");
    reader.open(QIODevice::ReadOnly);
    programInfo = QJsonDocument::fromJson(reader.readAll()).object();
    reader.close();
    reader.setFileName("./test_choices.json");
    reader.open(QIODevice::ReadOnly);
    testInfo = QJsonDocument::fromJson(reader.readAll()).object();
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
    ui->info_label->setText(infos[index]);
}


void ChoiceGuide::architectChosenSlot(QString name)
{
    //backend
    userChoice.architecture = name;
    userChoice.set = "";
    userChoice.programs.clear();

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
    refreshFinishState();
}

void ChoiceGuide::setChosenSlot(QString name)
{
    //backend
    userChoice.set = name;
    userChoice.programs.clear();
    //frontend
    ui->prog_list->clear();
    if(programInfo[userChoice.architecture].toObject()[name].isBool()){
        //not implemented
        return ;
    }
    auto programs = programInfo[userChoice.architecture].toObject()[name].toArray();

    for(auto program:programs){
        ui->prog_list->addItem(program.toString());
    }
    refreshFinishState();
}

void ChoiceGuide::programChosenSlot(QString name)
{
    //backend
    if(userChoice.programs.contains(name)){
        qDebug() << "remove " << name;
        userChoice.programs.removeOne(name);
    }
    else{
        qDebug() << "add " << name;
        userChoice.programs.push_back(name);
    }
    refreshFinishState();
}

void ChoiceGuide::testChosenSlot(QString name)
{
    //backend
    userChoice.test = name;
    refreshFinishState();
}

void ChoiceGuide::threadNumSetSlot(int threadNum)
{
    userChoice.threadNum = threadNum;
    refreshFinishState();
}

void ChoiceGuide::refreshFinishState()
{
    if(!userChoice.architecture.isEmpty()
        && !userChoice.set.isEmpty()
        && !userChoice.programs.isEmpty()
        && !userChoice.test.isEmpty()
        && !!userChoice.threadNum){
        ui->finish_button->setEnabled(true);
        return ;
    }
    ui->finish_button->setDisabled(true);
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
    qDebug() << userChoice.architecture;
    qDebug() << userChoice.set;
    qDebug() << userChoice.programs;
    qDebug() << userChoice.test;
    qDebug() << userChoice.threadNum;
}

