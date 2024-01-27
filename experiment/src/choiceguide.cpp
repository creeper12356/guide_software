#include "choiceguide.h"
#include "ui_choiceguide.h"

ChoiceGuide::ChoiceGuide() :
    QMainWindow(nullptr),
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

const Choice *ChoiceGuide::userChoice() const
{
    return &_userChoice;
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

    _userChoice.architecture = "";
    _userChoice.test = "";
    _userChoice.threadNum = 0;
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
    QFile reader("config/program_choices.json");
    reader.open(QIODevice::ReadOnly);
    programInfo = QJsonDocument::fromJson(reader.readAll()).object();
    reader.close();
    reader.setFileName("config/test_choices.json");
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
    _userChoice.set = "";
}

void ChoiceGuide::clearProgramChoice()
{
    ui->prog_list->clear();
    ui->select_all->setChecked(false);
    _userChoice.programs.clear();
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
    _userChoice.architecture = name;
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
    _userChoice.set = name;
    clearProgramChoice();
    if(programInfo[_userChoice.architecture].toObject()[name].isBool()){
        //not implemented
        return ;
    }

    auto programs = programInfo[_userChoice.architecture].toObject()[name].toArray();
    for(auto program:programs){
        ui->prog_list->addItem(program.toString());
    }
    refreshFinishState();
}

void ChoiceGuide::programChosenSlot()
{
    _userChoice.programs.clear();
    int selectedCount = ui->prog_list->selectedItems().size();
    for(auto* item: ui->prog_list->selectedItems()){
        _userChoice.programs.append(item->text());
    }
    refreshFinishState();
}

void ChoiceGuide::testChosenSlot(QString name)
{
    _userChoice.test = name;
    refreshFinishState();
}

void ChoiceGuide::threadNumSetSlot(int threadNum)
{
    _userChoice.threadNum = threadNum;
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
    if(!_userChoice.architecture.isEmpty()
        && !_userChoice.set.isEmpty()
        && !_userChoice.programs.isEmpty()
        && !_userChoice.test.isEmpty()
        && !!_userChoice.threadNum){
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
    emit configureFinished(&_userChoice);
    this->close();
}

