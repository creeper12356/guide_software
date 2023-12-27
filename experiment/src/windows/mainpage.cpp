#include "windows/mainpage.h"
#include "windows/aboutdialog.h"
#include "windows/choiceguide.h"
#include "widgets/choicewidget.h"
#include "widgets/imagedisplay.h"
#include "ui_mainpage.h"
#include "core.h"

MainPage::MainPage(Core *c, QWidget *parent) :
    QMainWindow(parent),
    core(c),
    ui(new Ui::MainPage)
{
    ui->setupUi(this);
    initToolBar();
    initDockWidgets();


    QHBoxLayout* centralLayout = new QHBoxLayout(ui->centralwidget);
    QSplitter* splitter = new QSplitter(Qt::Orientation::Horizontal,ui->centralwidget);

    heatMap = new ImageDisplay(ui->centralwidget);

    choiceWidget = new ChoiceWidget(ui->centralwidget);
    connect(choiceWidget,&ChoiceWidget::currentTextChanged,
            this,[this](const QString& program){
       heatMap->loadFromFile(QString("HeatMap/%1.png").arg(program));
    });

    heatMap->resize(choiceWidget->size());

    splitter->addWidget(choiceWidget);
    splitter->addWidget(heatMap);

    splitter->setStretchFactor(0,0);
    splitter->setStretchFactor(1,1);

    centralLayout->addWidget(splitter);
}

MainPage::~MainPage()
{
    qDebug() << "~MainPage()";
    delete ui;
}

QTextBrowser *MainPage::getTerminalReflect()
{
    return dynamic_cast<QTextBrowser*> (terminalDock->widget());
}

ChoiceWidget *MainPage::getChoiceWidget()
{
//    return dynamic_cast<ChoiceWidget*> (choiceDock->widget());
    return choiceWidget;
}

QTextBrowser *MainPage::getLogBrowser()
{
    return dynamic_cast<QTextBrowser*> (logDock->widget());
}

void MainPage::initToolBar()
{
    toolBar = new QToolBar(this);
    toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    this->addToolBar(Qt::TopToolBarArea,toolBar);
    toolBar->setIconSize(QSize(50,50));
    toolBar->addAction(ui->action_conf);
    toolBar->addAction(ui->action_clean);
    toolBar->addAction(ui->action_gen);
    toolBar->addAction(ui->action_sim);
    toolBar->addAction(ui->action_temp);
    toolBar->addAction(ui->action_terminate);
    ui->action_terminate->setDisabled(true);
}

void MainPage::initDockWidgets()
{
    QTextBrowser *terminalReflect = new QTextBrowser(this);
    terminalReflect->setStyleSheet("QTextBrowser{color:white;background:black}");
    (terminalDock = new QDockWidget("终端",this))->setWidget(terminalReflect);
    this->addDockWidget(Qt::BottomDockWidgetArea,terminalDock);

    QTextBrowser *logBrowser = new QTextBrowser(this);
//    logBrowser->setStyleSheet("QTextBrowser{color:black;background:white}");
    (logDock = new QDockWidget("日志",this))->setWidget(logBrowser);
    this->addDockWidget(Qt::BottomDockWidgetArea,logDock);

    this->tabifyDockWidget(terminalDock,logDock);

//    ChoiceWidget *choiceWidget = new ChoiceWidget(this);

//    (choiceDock = new QDockWidget("配置",this))->setWidget(choiceWidget);
//    this->addDockWidget(Qt::LeftDockWidgetArea,choiceDock);
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
    getLogBrowser()->clear();
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
        getLogBrowser()->clear();
        return ;
    }
    getLogBrowser()->append(info);
}

void MainPage::refreshLogProgram(QString program, QString info)
{
    QString prefix = "程序\"%1\": ";
    prefix = prefix.arg(program);
    getLogBrowser()->append(prefix + info);
}

void MainPage::on_action_maximize_triggered()
{
    if(this->isMaximized()){
        this->showNormal();
    }
    else{
        this->showMaximized();
    }
}

void MainPage::on_action_show_heatmap_triggered()
{
    //TODO: implement this
//    heatMapDock->setVisible(!heatMapDock->isVisible());
}
