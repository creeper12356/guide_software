#include "inc.h"
#include "dependencyinstaller.h"
#include "ui_dependencyinstaller.h"
#include "core.h"
DependencyInstaller::DependencyInstaller(Core *c, QEventLoop *&loop, QInputDialog *&pd, QWidget *parent)
   :QDialog(parent),
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

    //after user clicks the button box
    //the eventLoop quits
    connect(ui->button_box,&QDialogButtonBox::clicked,
           eventLoop,&QEventLoop::quit);
    connect(ui->button_box,&QDialogButtonBox::accepted,
            this,&DependencyInstaller::setAccepted);
}

DependencyInstaller::~DependencyInstaller()
{
    delete ui;
}

void DependencyInstaller::initCmds()
{
    }
bool DependencyInstaller::checkDependencies()
{
    if(proc->state() != QProcess::NotRunning){
        qDebug() << "The process is busy now.";
        return true;
    }

    proc->setArguments(QStringList() << "-c" << getNotInstalledCmd);
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
void DependencyInstaller::installDependencies(QString pwd)
{
    if(!isPasswdNeeded){
        pwd = "";
    }
    if(proc->state() != QProcess::NotRunning){
        qDebug() << "The process is busy now.";
        return ;
    }
    qDebug() << "start installing...";
    //update
    if(!updateCmd.isEmpty()){
        proc->setArguments(QStringList() << "-c" << updateCmd.arg(pwd));
        proc->start();
        eventLoop->exec();
        qDebug() << "update finished.";
    }
    int pkgCount = pkgList.count();
    int installCount = 0;
    for(auto pkgName:pkgList){
        proc->setArguments(QStringList() << "-c" << installCmd.arg(pwd,pkgName));
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
    //check dependencies first,
    //if all installed , return true
    if(checkDependencies()){
        return true;
    }
    //if not all installed
    while(true){
        //pre-set flags
        isAccepted = false;

        //wait for the user to accept or reject
        this->show();
        eventLoop->exec();
        if(!isAccepted){
            return false;
        }
        //the user accepts to install

        if(isPasswdNeeded){
            //wait for the user to input password or cancel
            pwdDialog->show();
            eventLoop->exec();
            if(pwdDialog->result() == QDialog::Rejected){
                continue;
            }
        }
        //the user inputs password
        this->switchInstallGUI();
        installDependencies(pwdDialog->textValue());

        //check if installation succeeds
        if(checkDependencies()){
            this->close();
            return true;
        }
    }
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
}

void DependencyInstaller::setAccepted()
{
    isAccepted = true;
}
AptInstaller::AptInstaller(Core *core, QEventLoop *&eventLoop, QInputDialog *&pwdDialog, QWidget *parent)
    :DependencyInstaller(core,eventLoop,pwdDialog,parent)
{
    initCmds();
    isPasswdNeeded = true;
}

void AptInstaller::initCmds()
{
    //specially used as sudo apt update
    updateCmd =
            "echo %1 | sudo -S apt update";
    //this command standard outputs a list of packages not installed
    getNotInstalledCmd =
            "xargs apt list --installed < requirements.txt "
            "| tail -n +2 "
            "| cut -f 1 -d / "
            "| sort "
            "> installed.txt "
            "&& "
            "comm installed.txt requirements.txt -13 "
            "&& "
            "rm installed.txt ";
    /*
     * this command allows sudo apt install without
     * manually input password in terminal
     */
   installCmd =
            "echo %1"
            " | sudo -S apt install %2 -y "
            "2> /dev/null";
}

PyLibInstaller::PyLibInstaller(Core *core, QEventLoop *&eventLoop, QInputDialog *&pwdDialog, QWidget *parent)
    :DependencyInstaller(core,eventLoop,pwdDialog,parent)
{
    initCmds();
    isPasswdNeeded = false;
    this->getUi()->hint_label->setText(
               "检测到您的电脑上没有安装下列Python库:");
}

void PyLibInstaller::initCmds()
{
    updateCmd = "";
    //this command standard outputs a list of packages not installed
    getNotInstalledCmd =
            "pip3 list | tail -n +3 | cut -f 1 -d ' ' | sort > py_installed.txt "
            "&& "
            "comm py_installed.txt py_requirements.txt -13 "
            "&& "
            "rm py_installed.txt ";
    installCmd =
            "%1pip3 install %2 "
            "2> /dev/null";
}
