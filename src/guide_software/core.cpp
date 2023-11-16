#include "inc.h"
#include "core.h"
#include "mainpage.h"
#include "ui_mainpage.h"
#include "dependencyinstaller.h"
#include "ui_dependencyinstaller.h"
#include "pylibinstaller.h"

Core::Core(QApplication* a):
    QObject(nullptr),
    app(a)
{
    configure();
    mainPage = new MainPage(this);
    //app backend quit iff the mainpage is closed
    connect(mainPage,&MainPage::closed,app,&QApplication::quit,Qt::QueuedConnection);
    installer = new DependencyInstaller(this);
    //init processes
    installerProc = new QProcess(this);
    installerProc->setProgram("bash");

    pwdDialog = new QInputDialog();
    pwdDialog->setLabelText("我们需要您提供用户的密码：");
    pwdDialog->setTextEchoMode(QLineEdit::Password);
//    pwdDialog->setModal(true);


    //connecting signals and slots
    //test
    connect(this,&Core::unmetDependencies,installer,&DependencyInstaller::showUnmetDependencies);
    connect(installer->getUi()->button_box,&QDialogButtonBox::accepted,
            this,&Core::askForPwd);
    connect(pwdDialog,&QInputDialog::textValueSelected,this,&Core::installDependencies);
    connect(this,&Core::installProcess,
            installer->getUi()->progress_bar,&QProgressBar::setValue);
    //test
    connect(installer->getUi()->button_box,&QDialogButtonBox::rejected,
            app,&QApplication::quit,Qt::QueuedConnection);
    connect(installer,&DependencyInstaller::allInstalled,
                mainPage,&MainPage::show);
    connect(installer,&DependencyInstaller::error,
            this,&Core::reportError);

    //check if all requirements are met.
    if(this->checkDependencies()){
        mainPage->show();
    }
//    if(!installer->checkDependencies()){
//        installer->show();
//    else{
//        mainPage->show();
//    }
}

Core::~Core()
{
    qDebug() << "~Core()";
    delete config;
    delete installer;
    delete mainPage;
    delete pwdDialog;

    delete installerProc;
}

bool Core::checkDependencies()
{
    if(installerProc->state() != QProcess::NotRunning){
        qDebug() << "The process is busy now.";
        return true;
    }

    //this command standard outputs a list of packages not installed
    QString getNotinstalledCmd =
            "xargs apt list --installed < requirements.txt "
            "| tail -n +2 "
            "| cut -f 1 -d / "
            "| sort "
            "> installed.txt "
            "&& "
            "comm installed.txt requirements.txt -13"
            "&& "
            "rm installed.txt ";
    installerProc->setArguments(QStringList() << "-c" << getNotinstalledCmd);
    installerProc->start();
    //this two wait will not timeout
    installerProc->waitForStarted(-1);
    installerProc->waitForFinished(-1);
    //run script finished
    pkgList.clear();
    //use QTextStream to read lines from QIODevice
    QTextStream stream(installerProc);
    while(!stream.atEnd()){
        pkgList.append(stream.readLine());
    }

    if(!pkgList.empty()){
        emit unmetDependencies(pkgList);
    }
    installerProc->close();
    return pkgList.empty();
}
void Core::installDependencies(const QString& pwd)
{
    pwdDialog->close();
    if(installerProc->state() != QProcess::NotRunning){
        qDebug() << "The process is busy now.";
        return ;
    }
    /*
     * first part : sudo apt install
     */
    /*
     *	TODO: check if password is right
     */
    /*
     * TODO: check if network is ok
     */
    //TODO: what if user enters wrong password?
    qDebug() << "start installing...";
    /* need sudo apt update */
    installerProc->setArguments(QStringList() << "-c" << "echo " + pwd + " | sudo -S apt update");
    installerProc->start();
    installerProc->waitForStarted(-1);
    installerProc->waitForFinished(-1);
    qDebug() << "update finished.";
    /*
     * this command allows sudo apt install without
     * manually input password in terminal
     */
    //TODO: redirect error output to /dev/null
    QString installCmd =
            "echo " + pwd +
            " | sudo -S apt install %1 -y "
            "2>> error.log";
    int pkgCount = pkgList.count();
    int installCount = 0;
    for(auto pkgName:pkgList){
        app->processEvents();
        installerProc->setArguments(QStringList() << "-c" << installCmd.arg(pkgName));
        installerProc->start();
        installerProc->waitForStarted(-1);
        installerProc->waitForFinished(-1);
        ++installCount;
        emit installProcess(100.0 * installCount / pkgCount);
        qDebug() << pkgName + " installed! ";
    }
    qDebug() << "successfully installed " << pkgList;
    installerProc->close();
    pkgList.clear();
    /*
     * second part: pip3 install
     */
    /*
     * TODO: check if all dependency installed again
     */
    if(checkDependencies()){
        qDebug() << "all installed";
        installer->close();
        mainPage->show();
    }
    else{
        qDebug() << "not successful";
        installer->switchCheckGUI();
    }
}
void Core::reportError(QString errMsg)
{
    QMessageBox::critical(nullptr,"error",errMsg);
    /*
     * the software will crash.
     */
    app->quit();
}

void Core::askForPwd()
{
    qDebug() << "accepted";
    installer->switchInstallGUI();
    pwdDialog->show();
}
void Core::configure()
{
    QFile reader("./config.json");
    reader.open(QIODevice::ReadOnly);
    if(!reader.isOpen()){
        qDebug() << "Exception: file not open";
    }
    config = new QJsonObject(QJsonDocument::fromJson(reader.readAll()).object());
    reader.close();
    qDebug() << "config = " << *config;
}
