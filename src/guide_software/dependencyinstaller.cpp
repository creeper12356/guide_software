#include "inc.h"
#include "dependencyinstaller.h"
#include "ui_dependencyinstaller.h"
#include "core.h"
DependencyInstaller::DependencyInstaller(Core *c, QEventLoop *&loop, QInputDialog *&pd, QWidget *parent) :
    QDialog(parent),
    core(c),
    eventLoop(loop),
    pwdDialog(pd),
    ui(new Ui::DependencyInstaller)
{
    ui->setupUi(this);
    //by default show check GUI
    this->switchCheckGUI();
    //init process
    proc = new QProcess(this);
    proc->setProgram("bash");

    //use event loop to avoid freezing GUIs
    connect(proc,SIGNAL(finished(int)),
            eventLoop,SLOT(quit()));
    connect(this,&DependencyInstaller::installProcess,
            ui->progress_bar,&QProgressBar::setValue);

}

DependencyInstaller::~DependencyInstaller()
{
    delete ui;
}
bool DependencyInstaller::checkDependencies()
{
    if(proc->state() != QProcess::NotRunning){
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
    proc->setArguments(QStringList() << "-c" << getNotinstalledCmd);
    proc->start();
    eventLoop->exec();
    pkgList.clear();

    //use QTextStream to read lines from QIODevice
    QTextStream stream(proc);
    while(!stream.atEnd()){
        pkgList.append(stream.readLine());
    }

    if(!pkgList.empty()){
        //find unmetDependencies
        showUnmetDependencies(pkgList);
    }
    proc->close();
    return pkgList.empty();
}
void DependencyInstaller::installDependencies(const QString& pwd)
{
    if(proc->state() != QProcess::NotRunning){
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
    qDebug() << "start installing...";
    /* need sudo apt update */
    proc->setArguments(QStringList() << "-c" << "echo " + pwd + " | sudo -S apt update");
    proc->start();
    eventLoop->exec();
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
        proc->setArguments(QStringList() << "-c" << installCmd.arg(pkgName));
        proc->start();
        eventLoop->exec();
        ++installCount;
    //GUI display intallation progress in real time
        emit installProcess(100.0 * installCount / pkgCount);
        qDebug() << pkgName + " installed! ";
    }
    qDebug() << "successfully installed " << pkgList;
    proc->close();
    pkgList.clear();
}

bool DependencyInstaller::checkAndInstall()
{
    if(checkDependencies()){
        //dependencies all installed
        return true;
    }
    //after user clicks the button box
    //the eventLoop quits
    connect(ui->button_box,&QDialogButtonBox::clicked,
           eventLoop,&QEventLoop::quit);
    connect(ui->button_box,&QDialogButtonBox::accepted,
            this,&DependencyInstaller::setAccepted);

    while(true){
        //pre-set
        isAccepted = false;
        this->show();
        eventLoop->exec();
        if(!isAccepted){
        /*TODO : improve it.
         */
        return false;
    }

    }
    //the user accepts to install

    //after the user inputs password ,
    //the event loop quits,
    //and installation process begins.

    pwdDialog->show();
    //when the user select or cancel the password dialog
    //the eventLoop quits
    connect(pwdDialog,&QInputDialog::finished,
            eventLoop,&QEventLoop::quit);
    eventLoop->exec();
    if(pwdDialog->result() == QDialog::Rejected){
        qDebug() << "password canceled out . ";
        return false;
    }
    //the user selects the password
    this->switchInstallGUI();
    installDependencies(pwdDialog->textValue());

    this->close();
    return checkDependencies();
}
void DependencyInstaller::switchCheckGUI()
{
    ui->hint_label->setText("检测到您的电脑上没有安装下列前置软件包:");
    ui->pkg_list->show();
    ui->q_label->show();
    ui->button_box->show();
    ui->progress_bar->hide();
}

void DependencyInstaller::switchInstallGUI()
{
    ui->hint_label->setText("正在安装中，这可能需要一点时间...");
    ui->pkg_list->hide();
    ui->q_label->hide();
    ui->button_box->hide();
    ui->progress_bar->setValue(0);
    ui->progress_bar->show();
}


void DependencyInstaller::showUnmetDependencies(const QStringList& list)
{
    this->switchCheckGUI();
    ui->pkg_list->clear();
    ui->pkg_list->addItems(list);
    this->show();
}

void DependencyInstaller::setAccepted()
{
    isAccepted = true;
}
