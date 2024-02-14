#include "mainpage.h"
#include "ui_mainpage.h"
#include "aboutdialog.h"
#include "choiceguide.h"
#include "choicewidget.h"
#include "consoledock.h"
#include "logdock.h"
#include "heatmapviewer.h"
#include "probewidget.h"

MainPage::MainPage() :
    QMainWindow(nullptr),
    ui(new Ui::MainPage)
{
    ui->setupUi(this);
    mGuide = new ChoiceGuide();
    initDockWidgets();
    initSplitter();
    restoreStateAndGeometry();
    initConnections();
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
    return ui->choiceWidget;
}

void MainPage::initDockWidgets()
{
    mConsoleDock = new ConsoleDock(this);
    mLogDock = new LogDock(this);

    addDockWidget(Qt::BottomDockWidgetArea, mConsoleDock);
    addDockWidget(Qt::BottomDockWidgetArea, mLogDock);
    tabifyDockWidget(mConsoleDock,mLogDock);

    mConsoleDock->setVisible(false);
    mLogDock->setVisible(false);

    auto toggleConsoleAction = mConsoleDock->toggleViewAction();
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

void MainPage::initSplitter()
{
    mLeftSplitter = new QSplitter(Qt::Vertical,this);
    mLeftSplitter->setObjectName("leftSplitter");
    mLeftSplitter->addWidget(ui->choiceWidget);
    mLeftSplitter->addWidget(ui->probeWidget);
    mMainSplitter  = new QSplitter(Qt::Horizontal,this);
    mMainSplitter->setObjectName("mainSplitter");

    mMainSplitter->addWidget(mLeftSplitter);
    mMainSplitter->addWidget(ui->heatMapViewer);

    centralWidget()->layout()->addWidget(mMainSplitter);
}

void MainPage::initConnections()
{
    //menuBar trigger
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

    //other trigger
    connect(ui->heatMapViewer,&HeatMapViewer::probeTriggered,this,&MainPage::probe);

    connect(ui->choiceWidget,&ChoiceWidget::currentTextChanged,
            this,[this](const QString& program){
       ui->heatMapViewer->open(QString("HeatMap/%1.png").arg(program));
    });
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

    QFile splitterRestorer;
    splitterRestorer.setFileName("config/mainSplitter.txt");
    if(splitterRestorer.open(QIODevice::ReadOnly)) {
        mMainSplitter->restoreState(splitterRestorer.readAll());
        splitterRestorer.close();
    }
    splitterRestorer.setFileName("config/leftSplitter.txt");
    if(splitterRestorer.open(QIODevice::ReadOnly)) {
        mLeftSplitter->restoreState(splitterRestorer.readAll());
        splitterRestorer.close();
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

    QFile splitterSaver;
    splitterSaver.setFileName("config/mainSplitter.txt");
    splitterSaver.open(QIODevice::WriteOnly);
    splitterSaver.write(mMainSplitter->saveState());
    splitterSaver.close();

    splitterSaver.setFileName("config/leftSplitter.txt");
    splitterSaver.open(QIODevice::WriteOnly);
    splitterSaver.write(mLeftSplitter->saveState());
    splitterSaver.close();
}

void MainPage::configureTriggered()
{
    QEventLoop eventLoop;
    connect(mGuide,&ChoiceGuide::configureFinished,&eventLoop,&QEventLoop::quit);
    mGuide->show();
    eventLoop.exec();
    ui->choiceWidget->refreshUserChoice(mGuide->userChoice());
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

void MainPage::displayProbeResult(qreal temperature, qreal probeX, qreal probeY)
{
    ui->probeWidget->setTemperature(temperature);
    ui->probeWidget->setProbeX(probeX);
    ui->probeWidget->setProbeY(probeY);
}

void MainPage::consoleAppendStdin(QString dir, QString info)
{
    mConsoleDock->appendStdin(dir,info);
}

void MainPage::consoleAppendStdout(QString info)
{
    mConsoleDock->appendStdout(info);
}

void MainPage::consoleAppendStderr(QString info)
{
    mConsoleDock->appendStderr(info);
}

void MainPage::longTaskStartedSlot()
{
    //forbid all actions but terminate when running long tasks
    mLogDock->clear();
    mConsoleDock->clear();
    ui->actionConfigure->setEnabled(false);
    ui->actionCleanScript->setEnabled(false);
    ui->actionGenScript->setEnabled(false);
    ui->actionSimulatePerformance->setEnabled(false);
    ui->actionGenHeatMap->setEnabled(false);
    ui->actionTerminate->setEnabled(true);
}

void MainPage::longTaskFinishedSlot()
{
    ui->actionConfigure->setEnabled(true);
    ui->actionCleanScript->setEnabled(true);
    ui->actionGenScript->setEnabled(true);
    ui->actionSimulatePerformance->setEnabled(true);
    ui->actionGenHeatMap->setEnabled(true);
    ui->actionTerminate->setEnabled(false);
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
        mLogDock->clear();
        return ;
    }
    mLogDock->append(info);
}

void MainPage::logConsoleProgram(const QString& program, const QString& info)
{
    QString prefix = "程序\"%1\": ";
    prefix = prefix.arg(program);
    mLogDock->append(prefix + info);
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
    ui->choiceWidget->refreshUserChoice(userChoice);
}
