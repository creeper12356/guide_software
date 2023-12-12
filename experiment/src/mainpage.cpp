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

    connect(ui->action_full_screen,&QAction::triggered,this,&MainPage::switchFullScreen);
    QDockWidget* terminalDock = new QDockWidget(this);
    terminalDock->setWindowTitle("终端");
    terminalDock->setWidget(ui->terminal_reflect);
    QDockWidget* choiceDock = new QDockWidget(this);
    choiceDock->setWidget(ui->choice_widget);
    choiceDock->setWindowTitle("当前配置");
    QDockWidget* logDock = new QDockWidget(this);
    logDock->setWindowTitle("日志");
    logDock->setWidget(ui->log_browser);
    this->addDockWidget(Qt::RightDockWidgetArea,terminalDock);
    this->addDockWidget(Qt::LeftDockWidgetArea,choiceDock);
    this->addDockWidget(Qt::BottomDockWidgetArea,logDock);
    //TODO: 整理
    QDockWidget* imgDock = new QDockWidget(this);
    imgDock->setWindowTitle("温度仿真");
    imgDock->show();
    imgDock->setWidget(ui->image_display);
    this->addDockWidget(Qt::NoDockWidgetArea,imgDock);
    ui->image_display->loadFromFile("HeatMap/blackscholes.png");

}

MainPage::~MainPage()
{
    qDebug() << "~MainPage()";
    delete ui;
}

void MainPage::closeEvent(QCloseEvent *event)
{
    if(core->isProcessRunning()){
        if(QMessageBox::warning(this,"警告",
                             "有一个进程正在运行，您未完成的工作可能丢失，仍然关闭?",
                             QMessageBox::Yes|QMessageBox::No)
                == QMessageBox::Yes){
            //用户强行关闭进程
            //kill进程
            core->terminate();
        }
        else{
            //用户取消强行关闭
            event->ignore();
            return ;
        }
    }
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

void MainPage::switchFullScreen()
{
    if(this->isFullScreen()){
        this->showNormal();
    }
    else{
        this->showFullScreen();
    }
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
    ui->log_browser->clear();
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
}

void MainPage::refreshLog(QString info)
{
    if(info.isEmpty()){
        //若信息为空则清空日志
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
