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
    toolBar = new QToolBar(this);
    this->addToolBar(toolBar);
    toolBar->setIconSize(QSize(50,50));
    toolBar->addAction(ui->action_conf);
    toolBar->addAction(ui->action_clean);
    toolBar->addAction(ui->action_gen);
    toolBar->addAction(ui->action_sim);
    toolBar->addAction(ui->action_temp);
    toolBar->addAction(ui->action_terminate);
    ui->action_terminate->setDisabled(true);
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
    ui->statusbar->showMessage("清理脚本完成",3 * SECOND);
}

void MainPage::scriptGeneratedSlot()
{
    ui->statusbar->showMessage("脚本已成功生成",3 * SECOND);
}

void MainPage::performanceSimulationFinishedSlot()
{
    ui->statusbar->showMessage("性能仿真结束",3 * SECOND);
}

void MainPage::longTaskStartedSlot()
{
    //forbid all actions but terminate when running long tasks
    for(auto action: toolBar->actions()){
        action->setDisabled(true);
    }
    ui->action_terminate->setEnabled(true);
}

void MainPage::longTaskFinishedSlot()
{
    for(auto action: toolBar->actions()){
        action->setEnabled(true);
    }
    ui->action_terminate->setDisabled(true);
    //TODO
}

void MainPage::refreshLog(QString info)
{
    if(info.isEmpty()){
        ui->log_browser->clear();
        return ;
    }
    ui->log_browser->append(info);
}

void MainPage::refreshLogProgram(QString program, QString info)
{
    QString prefix = "程序\"%1\": ";
    prefix = prefix.arg(program);
    ui->log_browser->append(prefix + info);
}
