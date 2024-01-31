#include "mainpage.h"
#include "ui_mainpage.h"
#include "aboutdialog.h"
#include "choiceguide.h"
#include "choicewidget.h"
#include "consoledock.h"
#include "consoledock.h"
#include "heatmapviewer.h"

MainPage::MainPage() :
    QMainWindow(nullptr),
    ui(new Ui::MainPage)
{
    ui->setupUi(this);

    QHBoxLayout* centralLayout = new QHBoxLayout(ui->centralwidget);

    initToolBar();
    initDockWidgets();


    mChoiceWidget = new ChoiceWidget(ui->centralwidget);
    connect(mChoiceWidget,&ChoiceWidget::currentTextChanged,
            this,[this](const QString& program){
       mHeatMapViewer->open(QString("HeatMap/%1.png").arg(program));
    });

    mGuide = new ChoiceGuide();
    connect(ui->actionConfigure,&QAction::triggered,mGuide,&ChoiceGuide::show);

    mHeatMapViewer = new HeatMapViewer(this);
//    mHeatMapDisplay->resize(mChoiceWidget->size());

    QSplitter* splitter = new QSplitter(Qt::Orientation::Horizontal,ui->centralwidget);
    splitter->addWidget(mChoiceWidget);
    splitter->addWidget(mHeatMapViewer);
    splitter->setStretchFactor(0,0);
    splitter->setStretchFactor(1,1);
    centralLayout->addWidget(splitter);

    restoreStateAndGeometry();

    connect(ui->actionQuit,&QAction::triggered,this,&MainPage::quit);

    connect(ui->actionConfigure,&QAction::triggered,this,&MainPage::configureTriggered);
    connect(ui->actionClearConfig,&QAction::triggered,this,&MainPage::clearConfig);
    connect(ui->actionCleanScript,&QAction::triggered,this,&MainPage::cleanScript);
    connect(ui->actionGenScript,&QAction::triggered,this,&MainPage::genScript);

    connect(ui->actionSimulatePerformance,&QAction::triggered,
            this,&MainPage::simulatePerformanceTriggered);
    connect(ui->actionGenHeatMap,&QAction::triggered,this,&MainPage::genHeatMapTriggered);

    connect(ui->actionTerminate,&QAction::triggered,this,&MainPage::terminate);

    connect(ui->actionMaximize,&QAction::triggered,this,&MainPage::maximizeTriggered);
    connect(ui->actionAbout,&QAction::triggered,this,&MainPage::aboutTriggererd);
    connect(ui->actionAboutqt,&QAction::triggered,this,&MainPage::aboutqtTriggered);

    connect(mHeatMapViewer,&HeatMapViewer::probeTriggered,this,&MainPage::probe);
}

MainPage::~MainPage()
{
    qDebug() << "~MainPage()";
    saveStateAndGeometry();
    delete mGuide;
    delete ui;
}

ConsoleDock *MainPage::consoleDock()
{
    return mConsoleDock;
}

ChoiceWidget *MainPage::choiceWidget()
{
    return mChoiceWidget;
}

QTextBrowser *MainPage::logBrowser()
{
    return dynamic_cast<QTextBrowser*> (mLogDock->widget());
}

void MainPage::initToolBar()
{
    mToolBar = new QToolBar(this);
    mToolBar->setObjectName("toolBar");
    mToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    this->addToolBar(Qt::TopToolBarArea,mToolBar);
    mToolBar->setIconSize(QSize(30,30));

    mToolBar->addAction(ui->actionConfigure);
    mToolBar->addAction(ui->actionCleanScript);
    mToolBar->addAction(ui->actionGenScript);
    mToolBar->addAction(ui->actionSimulatePerformance);
    mToolBar->addAction(ui->actionGenHeatMap);
    mToolBar->addAction(ui->actionTerminate);

    auto font = mToolBar->font();
    mToolBar->setFont(font);
    ui->actionTerminate->setDisabled(true);
}

void MainPage::initDockWidgets()
{
    mConsoleDock = new ConsoleDock(this);
    QTextBrowser *logBrowser = new QTextBrowser(this);
    (mLogDock = new QDockWidget("日志",this))->setWidget(logBrowser);
    mLogDock->setObjectName("logDock");

    addDockWidget(Qt::BottomDockWidgetArea, mConsoleDock);
    addDockWidget(Qt::BottomDockWidgetArea, mLogDock);
    tabifyDockWidget(mConsoleDock,mLogDock);

    mConsoleDock->setVisible(false);
    mLogDock->setVisible(false);

    auto toggleConsoleAction = mConsoleDock->toggleViewAction();
//    toggleConsoleAction->setIcon(terminalIcon);
//    toggleConsoleAction->setIconVisibleInMenu(false);
    auto consoleToggleButton = new QToolButton;
    consoleToggleButton->setDefaultAction(toggleConsoleAction);
    consoleToggleButton->setAutoRaise(true);
    connect(toggleConsoleAction, &QAction::toggled, this, [this] (bool checked) {
        if (checked) {
            mConsoleDock->show();
            if (!mLogDock->isFloating() && tabifiedDockWidgets(mConsoleDock).contains(mLogDock))
                mLogDock->hide();
            mConsoleDock->raise();
        }
    });

    auto toggleLogAction = mLogDock->toggleViewAction();
    auto logToggleButton = new QToolButton;
    logToggleButton->setDefaultAction(toggleLogAction);
    logToggleButton->setAutoRaise(true);
    connect(toggleLogAction, &QAction::toggled, this, [this] (bool checked) {
        if (checked) {
            mLogDock->show();
            if (!mConsoleDock->isFloating() && tabifiedDockWidgets(mLogDock).contains(mConsoleDock))
                mConsoleDock->hide();
            mLogDock->raise();
        }
    });
    ui->statusbar->addWidget(consoleToggleButton);
    ui->statusbar->addWidget(logToggleButton);
}

void MainPage::restoreStateAndGeometry()
{
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

void MainPage::saveStateAndGeometry()
{
    QFile stateSaver("config/state.txt");
    stateSaver.open(QIODevice::WriteOnly);
    stateSaver.write(this->saveState());
    stateSaver.close();
    QFile geometrySaver("config/geometry.txt");
    geometrySaver.open(QIODevice::WriteOnly);
    geometrySaver.write(this->saveGeometry());
    geometrySaver.close();
}

void MainPage::configureTriggered()
{
    QEventLoop eventLoop;
    connect(mGuide,&ChoiceGuide::configureFinished,&eventLoop,&QEventLoop::quit);
    mGuide->show();
    eventLoop.exec();
    mChoiceWidget->refreshUserChoice(mGuide->userChoice());
    emit configureFinished(mGuide->userChoice());
}

void MainPage::simulatePerformanceTriggered()
{
    ui->actionSimulatePerformance->setEnabled(false);
    emit simulatePerformance();
}

void MainPage::genHeatMapTriggered()
{
    ui->actionGenHeatMap->setEnabled(false);
    emit genHeatMap();
}

void MainPage::aboutTriggererd()
{
    QMessageBox::about(this, "关于",
                     "<center><h2>Guide Software</h2></center><br>"
                     "热仿真向导软件<br>"
                     "Source code at <a style=\"color: #8AB8FE\" "
                     "href='https://github.com/creeper12356/guide_software'>GitHub</a>");
}

void MainPage::cleanScriptFinishedSlot()
{
    logConsole("清理脚本完成。");
}

void MainPage::genScriptFinishedSlot()
{
    logConsole("脚本已成功生成。");
}

void MainPage::genScriptFailedSlot(QString warningInfo)
{
    warning(warningInfo);
}

void MainPage::performanceSimulationFinishedSlot()
{
    logConsole("性能仿真结束。");
}

void MainPage::performanceSimulationFailedSlot(QString warningInfo)
{
    warning(warningInfo);
    ui->actionSimulatePerformance->setEnabled(true);
}

void MainPage::genHeatMapFinishedSlot()
{
    logConsole("生成温度图结束。");
}

void MainPage::longTaskStartedSlot()
{
    //forbid all actions but terminate when running long tasks
    logBrowser()->clear();
    for(auto action: mToolBar->actions()){
        action->setDisabled(true);
    }
    ui->actionTerminate->setEnabled(true);
}

void MainPage::longTaskFinishedSlot()
{
    for(auto action: mToolBar->actions()){
        action->setEnabled(true);
    }
    ui->actionTerminate->setDisabled(true);
}

void MainPage::askQuitSlot()
{
    auto button = QMessageBox::warning(this,"警告","存在未结束的进程，是否强制退出？",QMessageBox::No | QMessageBox::Yes);
    if(button == QMessageBox::Yes){
        emit forceQuit();
    }
}

void MainPage::warning(const QString &info)
{
    QMessageBox::warning(this,"警告",info);
}

void MainPage::critical(const QString &info)
{
    QMessageBox::critical(this,"错误",info);
}

void MainPage::logConsole(const QString &info)
{
    if(info.isEmpty()){
        //若信息为空则清空日志
        logBrowser()->clear();
        return ;
    }
    logBrowser()->append(info);
}

void MainPage::logConsoleProgram(const QString& program, const QString& info)
{
    QString prefix = "程序\"%1\": ";
    prefix = prefix.arg(program);
    logBrowser()->append(prefix + info);
}

void MainPage::closeEvent(QCloseEvent *event)
{
    ui->actionQuit->trigger();
    event->ignore();
}


void MainPage::maximizeTriggered()
{
    if(isMaximized()) showNormal();
    else			  showMaximized();
}

void MainPage::aboutqtTriggered()
{
    QMessageBox::aboutQt(this,"关于Qt");
}

void MainPage::updateUserChoice(const Choice *userChoice)
{
    mChoiceWidget->refreshUserChoice(userChoice);
}
