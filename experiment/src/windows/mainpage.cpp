#include "windows/mainpage.h"
#include "windows/aboutdialog.h"
#include "windows/choiceguide.h"
#include "widgets/choicewidget.h"
#include "widgets/imagedisplay.h"
#include "widgets/consoledock.h"
#include "ui_mainpage.h"
#include "core.h"
#include "widgets/consoledock.h"


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

    QFile stateRestorer("config/state.txt");
    if(stateRestorer.open(QIODevice::ReadOnly)){
        this->restoreState(stateRestorer.readAll());
        stateRestorer.close();
    }
    QFile geometryRestorer("config/geometry.txt");
    if(geometryRestorer.open(QIODevice::ReadOnly)){
        this->restoreGeometry(geometryRestorer.readAll());
        geometryRestorer.close();
    }
}

MainPage::~MainPage()
{
    qDebug() << "~MainPage()";

    QFile stateSaver("config/state.txt");
    stateSaver.open(QIODevice::WriteOnly);
    stateSaver.write(this->saveState());
    stateSaver.close();
    QFile geometrySaver("config/geometry.txt");
    geometrySaver.open(QIODevice::WriteOnly);
    geometrySaver.write(this->saveGeometry());
    geometrySaver.close();

    delete ui;
}

ConsoleDock *MainPage::getConsoleDock()
{
    return consoleDock;
}

ChoiceWidget *MainPage::getChoiceWidget()
{
    return choiceWidget;
}

QTextBrowser *MainPage::getLogBrowser()
{
    return dynamic_cast<QTextBrowser*> (logDock->widget());
}

void MainPage::initToolBar()
{
    toolBar = new QToolBar(this);
    toolBar->setObjectName("toolBar");
    toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    this->addToolBar(Qt::TopToolBarArea,toolBar);
    toolBar->setIconSize(QSize(40,40));
    connect(ui->action_sim,&QAction::triggered,[this](){
       ui->action_sim->setEnabled(false);
    });
    connect(ui->action_temp,&QAction::triggered,[this](){
        ui->action_temp->setEnabled(false);
    });
    toolBar->addAction(ui->action_conf);
    toolBar->addAction(ui->action_clean);
    toolBar->addAction(ui->action_gen);
    toolBar->addAction(ui->action_sim);
    toolBar->addAction(ui->action_temp);
    toolBar->addAction(ui->action_terminate);

    auto font = toolBar->font();
    toolBar->setFont(font);
    ui->action_terminate->setDisabled(true);
}

void MainPage::initDockWidgets()
{
    consoleDock = new ConsoleDock(this);
    QTextBrowser *logBrowser = new QTextBrowser(this);
    (logDock = new QDockWidget("日志",this))->setWidget(logBrowser);
    logDock->setObjectName("logDock");

    addDockWidget(Qt::BottomDockWidgetArea, consoleDock);
    addDockWidget(Qt::BottomDockWidgetArea, logDock);
    tabifyDockWidget(consoleDock,logDock);

    consoleDock->setVisible(false);
    logDock->setVisible(false);

    auto toggleConsoleAction = consoleDock->toggleViewAction();
//    toggleConsoleAction->setIcon(terminalIcon);
//    toggleConsoleAction->setIconVisibleInMenu(false);
    auto consoleToggleButton = new QToolButton;
    consoleToggleButton->setDefaultAction(toggleConsoleAction);
    consoleToggleButton->setAutoRaise(true);
    connect(toggleConsoleAction, &QAction::toggled, this, [this] (bool checked) {
        if (checked) {
            consoleDock->show();
            if (!logDock->isFloating() && tabifiedDockWidgets(consoleDock).contains(logDock))
                logDock->hide();
            consoleDock->raise();
        }
    });

    auto toggleLogAction = logDock->toggleViewAction();
    auto logToggleButton = new QToolButton;
    logToggleButton->setDefaultAction(toggleLogAction);
    logToggleButton->setAutoRaise(true);
    connect(toggleLogAction, &QAction::toggled, this, [this] (bool checked) {
        if (checked) {
            logDock->show();
            if (!consoleDock->isFloating() && tabifiedDockWidgets(logDock).contains(consoleDock))
                consoleDock->hide();
            logDock->raise();
        }
    });
    ui->statusbar->addWidget(consoleToggleButton);
    ui->statusbar->addWidget(logToggleButton);
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
    logConsole("清理脚本完成。");
}

void MainPage::scriptGeneratedSlot()
{
    logConsole("脚本已成功生成。");
}

void MainPage::performanceSimulationFinishedSlot()
{
    logConsole("性能仿真结束。");
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

void MainPage::logConsole(const QString &info)
{
    if(info.isEmpty()){
        //若信息为空则清空日志
        getLogBrowser()->clear();
        return ;
    }
    getLogBrowser()->append(info);
}

void MainPage::logConsoleProgram(const QString& program, const QString& info)
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

void MainPage::on_action_aboutqt_triggered()
{
    QMessageBox::aboutQt(this,"关于Qt");
}




