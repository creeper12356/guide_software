#include "mainpage.h"
#include "ui_mainpage.h"
#include "aboutdialog.h"
#include "choiceguide.h"
#include "choicewidget.h"
#include "imagedisplay.h"
#include "consoledock.h"
#include "consoledock.h"


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
       mHeatMapDisplay->loadFromFile(QString("HeatMap/%1.png").arg(program));
    });

    mGuide = new ChoiceGuide();
    connect(ui->actionConfigure,&QAction::triggered,mGuide,&ChoiceGuide::show);

    mHeatMapDisplay = new ImageDisplay(ui->centralwidget);
    mHeatMapDisplay->resize(mChoiceWidget->size());

    QSplitter* splitter = new QSplitter(Qt::Orientation::Horizontal,ui->centralwidget);
    splitter->addWidget(mChoiceWidget);
    splitter->addWidget(mHeatMapDisplay);
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
    mToolBar->setIconSize(QSize(40,40));

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
    AboutDialog aboutDialog;
    aboutDialog.exec();
}

void MainPage::cleanScriptFinishedSlot()
{
    logConsole("清理脚本完成。");
}

void MainPage::genScriptFinishedSlot()
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