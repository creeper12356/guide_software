#include "choiceguide.h"
#include "ui_choiceguide.h"
#include "core.h"

ChoiceGuide::ChoiceGuide(Core *c, QWidget *parent) :
    QMainWindow(parent),
    core(c),
    ui(new Ui::ChoiceGuide)
{
    ui->setupUi(this);

    loadChoices();

    initArchitectButtons();
    initSetButtons();
    initTestList();
    connect(ui->prog_list,&QListWidget::itemSelectionChanged,this,&ChoiceGuide::programChosenSlot);
    connect(ui->select_all,&QCheckBox::stateChanged,this,&ChoiceGuide::selectAllPrograms);
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
    //重置按钮状态
    //重置所有架构选择按钮为未按下
    archGroup->setExclusive(false);
    for(auto* button:archGroup->buttons()){
        button->setChecked(false);
    }
    archGroup->setExclusive(true);

    //重置控制按钮状态
    ui->prev_button->setEnabled(false);
    ui->next_button->setEnabled(true);
    ui->finish_button->setEnabled(false);

    clearSetChoice();
    clearProgramChoice();

    //重置控件
    ui->test_list->setCurrentRow(-1);
    ui->thread_num_box->setValue(0);

    userChoice.architecture = "";
    userChoice.test = "";
    userChoice.threadNum = 0;
}

void ChoiceGuide::initArchitectButtons()
{
    archGroup = new QButtonGroup(this);
    archGroup->setExclusive(true);
    int count = 0;
    for(auto key:programInfo.keys()){
        QPushButton* newButton = new QPushButton(key,this);
        archGroup->addButton(newButton);
        newButton->setCheckable(true);
        ui->arch_grid->addWidget(newButton,0,count);
        ++count;
        connect(newButton,&QPushButton::clicked,this,[newButton,this](){
            this->architectChosenSlot(newButton->text());
            qDebug() << newButton->text();
        });
    }
}

void ChoiceGuide::initSetButtons()
{
    setGroup = new QButtonGroup(this);
    setGroup->setExclusive(true);
}

void ChoiceGuide::initTestList()
{
    for(auto key:testInfo.keys()){
        ui->test_list->addItem(key);
    }
    connect(ui->test_list,&QListWidget::itemClicked,this,[this](QListWidgetItem* item){
        qDebug() << item->text();
        this->testChosenSlot(item->text());
    });

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

void ChoiceGuide::clearSetChoice()
{
    //清空程序集选择按钮组
    while(setGroup->buttons().size()){
        setGroup->removeButton(setGroup->buttons()[0]);
    }
    //删除程序集选择按钮
    while(ui->set_grid->count()){
        delete ui->set_grid->takeAt(0)->widget();
    }
    //后端清除
    userChoice.set = "";
}

void ChoiceGuide::clearProgramChoice()
{
    ui->prog_list->clear();
    ui->select_all->setChecked(false);
    userChoice.programs.clear();
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
    userChoice.architecture = name;
    clearSetChoice();
    clearProgramChoice();

    if(programInfo[name].isBool()){
        //not implemented
        qDebug() << "not implemented yet . return ";
        return ;
    }
    auto sets = programInfo[name].toObject();
    for(auto key:sets.keys()){
        QPushButton *newButton = new QPushButton(key,this);
        newButton->setCheckable(true);
        setGroup->addButton(newButton);
        ui->set_grid->addWidget(newButton);
        connect(newButton,&QPushButton::clicked,this,[this,newButton](){
           this->setChosenSlot(newButton->text());
        });
    }
    refreshFinishState();
}

void ChoiceGuide::setChosenSlot(QString name)
{
    userChoice.set = name;
    clearProgramChoice();
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

void ChoiceGuide::programChosenSlot()
{
    userChoice.programs.clear();
    int selectedCount = ui->prog_list->selectedItems().size();
    for(auto* item: ui->prog_list->selectedItems()){
        userChoice.programs.append(item->text());
    }
    refreshFinishState();
}

void ChoiceGuide::testChosenSlot(QString name)
{
    userChoice.test = name;
    refreshFinishState();
}

void ChoiceGuide::threadNumSetSlot(int threadNum)
{
    userChoice.threadNum = threadNum;
    refreshFinishState();
}

void ChoiceGuide::selectAllPrograms(bool flag)
{
    qDebug() << flag;
    int count = ui->prog_list->count();
    for(int i = 0;i < count;++i){
        ui->prog_list->item(i)->setSelected(flag);
    }
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
    //丢弃所有的更改
    this->close();
}

void ChoiceGuide::on_finish_button_clicked()
{
    //保存所有的更改
    core->copyUserChoice(&userChoice);
    qDebug() << "saved!";
    this->close();
}

