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

void MainPage::closeEvent(QCloseEvent *event)
{
    emit closed();
}
