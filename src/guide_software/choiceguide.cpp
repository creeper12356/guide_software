#include "choiceguide.h"
#include "ui_choiceguide.h"

ChoiceGuide::ChoiceGuide(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ChoiceGuide)
{
    ui->setupUi(this);

    //read from program_choices.json
    QFile reader("./program_choices.json");
    reader.open(QIODevice::ReadOnly);
    QJsonObject object = QJsonDocument::fromJson(reader.readAll()).object();
    qDebug() << object;
    int count = 0;
    for(auto key:object.keys()){
        ui->gridLayout->addWidget(new QPushButton(key,this),0,count);
        ++count;
    }
}

ChoiceGuide::~ChoiceGuide()
{
    delete ui;
}
