#include "inc.h"
#include "mainpage.h"
#include "ui_mainpage.h"
#include "core.h"
#include "aboutdialog.h"
#include "choiceguide.h"

MainPage::MainPage(Core *c, QWidget *parent) :
    QMainWindow(parent),
    core(c),
    ui(new Ui::MainPage)
{
    ui->setupUi(this);
    //初始化toolBar
    QToolBar* toolBar = new QToolBar(this);
    this->addToolBar(toolBar);
    toolBar->setIconSize(QSize(50,50));
    toolBar->addAction(ui->action_conf);
    toolBar->addAction(ui->action_clean);
    toolBar->addAction(ui->action_gen);
    toolBar->addAction(ui->action_sim);

}

MainPage::~MainPage()
{
    qDebug() << "~MainPage()";
    delete ui;
}

void MainPage::closeEvent(QCloseEvent *event)
{
    emit closed();
}

void MainPage::on_action_about_triggered()
{
    AboutDialog aboutDialog(this);
    aboutDialog.exec();
}

void MainPage::on_action_exit_triggered()
{
    this->close();
}

void MainPage::scriptCleanedSlot()
{
    qDebug() << "script cleaned.";
    ui->statusbar->showMessage("清理脚本完成",3 * SECOND);
}

void MainPage::scriptGeneratedSlot()
{
    qDebug() << "script generated.";
    ui->statusbar->showMessage("脚本已成功生成",3 * SECOND);
}

void MainPage::refreshUserChoice(const Choice *userChoice)
{
    ui->prog_list->clear();
    ui->prog_list->addItems(userChoice->programs);
    ui->test_label->setText(userChoice->test);
    ui->thread_num_label->setText(QString::number(userChoice->threadNum));
}
