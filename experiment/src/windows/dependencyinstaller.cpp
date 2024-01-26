#include "windows/dependencyinstaller.h"
#include "passworddialog.h"
#include "core.h"
#include "ui_dependencyinstaller.h"
DependencyInstaller::DependencyInstaller(Core *c, QEventLoop *loop, PasswordDialog *&pd, QWidget *parent)
   :QDialog(parent),
    core(c),
    eventLoop(loop),
    pwdDialog(pd),
    ui(new Ui::DependencyInstaller)
{
    ui->setupUi(this);
    //默认显示检查GUI
    this->switchCheckGUI();

    proc = new QProcess(this);
    proc->setProgram("bash");

    connect(proc,SIGNAL(finished(int)),
            eventLoop,SLOT(quit()));
    connect(this,&DependencyInstaller::installProcess,
            ui->progress_bar,&QProgressBar::setValue);

    //用户点击button box后，事件循环终止
    connect(ui->button_box,&QDialogButtonBox::clicked,
           eventLoop,&QEventLoop::quit);
    connect(ui->button_box,&QDialogButtonBox::accepted,
            this,&DependencyInstaller::setAccepted);
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

    proc->setArguments(QStringList() << "-c" << getNotInstalledCmd);
    proc->start();
    eventLoop->exec();
    pkgList.clear();

    QTextStream stream(proc);
    while(!stream.atEnd()){
        pkgList.append(stream.readLine());
    }

    if(!pkgList.empty()){
        //存在未满足依赖
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
        return true;
    }
    while(true){
        //pre-set flags
        isAccepted = false;

        this->show();
        eventLoop->exec();
        if(!isAccepted){
            return false;
        }
        //用户接受安装

        if(isPasswdNeeded){
            //等待用户输入密码
            pwdDialog->show();
            eventLoop->exec();
            if(pwdDialog->result() == QDialog::Rejected){
                continue;
            }
        }
        this->switchInstallGUI();
        installDependencies(pwdDialog->textValue());

        //再次检查依赖是否安装
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
AptInstaller::AptInstaller(Core *core, QEventLoop *eventLoop, PasswordDialog *&pwdDialog, QWidget *parent)
    :DependencyInstaller(core,eventLoop,pwdDialog,parent)
{
    initCmds();
    isPasswdNeeded = true;
}

void AptInstaller::initCmds()
{
    updateCmd =
            "echo %1 | sudo -S apt update";
    getNotInstalledCmd =
            "xargs apt list --installed < config/requirements.txt "
            "| tail -n +2 "
            "| cut -f 1 -d / "
            "| sort "
            "> config/installed.txt "
            "&& "
            "comm config/installed.txt config/requirements.txt -13 "
            "&& "
            "rm config/installed.txt ";
   installCmd =
            "echo %1"
            " | sudo -S apt install %2 -y "
            "2> /dev/null";
}

PyLibInstaller::PyLibInstaller(Core *core, QEventLoop *eventLoop, PasswordDialog *&pwdDialog, QWidget *parent)
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
    getNotInstalledCmd =
            "pip list | tail -n +3 | cut -f 1 -d ' ' | sort > config/py_installed.txt "
            "&& "
            "comm config/py_installed.txt config/py_requirements.txt -13 "
            "&& "
            "rm config/py_installed.txt ";
    installCmd =
            "%1pip install %2 "
            "2> /dev/null";
}
