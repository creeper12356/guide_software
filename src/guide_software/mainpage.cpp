#include "inc.h"
#include "mainpage.h"
#include "ui_mainpage.h"
#include "core.h"

MainPage::MainPage(Core *c, QWidget *parent) :
    QMainWindow(parent),
    core(c),
    ui(new Ui::MainPage)
{
    ui->setupUi(this);
}

MainPage::~MainPage()
{
    qDebug() << "~MainPage()";
    delete ui;
}

void MainPage::on_exit_button_clicked()
{
    this->close();
}

void MainPage::displayWritingScript(bool state)
{
    if(state)
        ui->gen_button->setStyleSheet("background-color: green");
    else
        ui->gen_button->setStyleSheet("background-color: red");
    QTimer::singleShot(1 * SECOND,this,[this](){
        ui->gen_button->setStyleSheet("");
    });
}

void MainPage::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape){
        this->close();
    }
}

void MainPage::closeEvent(QCloseEvent *event)
{
    emit closed();
}
