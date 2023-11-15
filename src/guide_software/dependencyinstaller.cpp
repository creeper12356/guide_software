#include "inc.h"
#include "dependencyinstaller.h"
#include "ui_dependencyinstaller.h"
#include "core.h"
DependencyInstaller::DependencyInstaller(Core *c, QWidget *parent) :
    QDialog(parent),
    core(c),
    ui(new Ui::DependencyInstaller)
{
    ui->setupUi(this);
    //by default show check GUI
    this->switchCheckGUI();

    proc = new QProcess(this);
    proc->setProgram("bash");

    pwdDialog = new QInputDialog(this);
    pwdDialog->setLabelText("我们需要您提供用户的密码：");
    pwdDialog->setTextEchoMode(QLineEdit::Password);
    pwdDialog->setModal(true);

    ui->progress_bar->hide();
}

DependencyInstaller::~DependencyInstaller()
{
    delete ui;
}
bool DependencyInstaller::checkDependencies()
{
    if(proc->state() != QProcess::NotRunning){
        qDebug() << "DInstaller::checkD: The process is busy now.";
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
    //this two wait will not timeout
    proc->waitForStarted(-1);
    proc->waitForFinished(-1);
    //run script finished
    pkgList.clear();
    //use QTextStream to read lines from QIODevice
    QTextStream stream(proc);
    while(!stream.atEnd()){
        pkgList.append(stream.readLine());
    }

    if(pkgList.empty()){
        proc->close();
        return true;
    }
    ui->pkg_list->clear();
    ui->pkg_list->addItems(pkgList);
    proc->close();
    return false;
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

void DependencyInstaller::installDependencies(const QString& pwd)
{
    /*
     * first part : sudo apt install
     */
    /*
     *	TODO: check if password is right
     */
    /*
     * TODO: check if network is ok
     */
    pwdDialog->close();
    //TODO: what if user enters wrong password?
    qDebug() << "start installing...";
    /* need sudo apt update */
    proc->setArguments(QStringList() << "-c" << "echo " + pwd + " | sudo -S apt update");
    proc->start();
    proc->waitForStarted(-1);
    proc->waitForFinished(-1);
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
        qDebug() << "proc.arg" << proc->arguments();
        proc->start();
        proc->waitForStarted(-1);
        proc->waitForFinished(-1);
        ++installCount;
        ui->progress_bar->setValue(100.0 * installCount / pkgCount);
        qDebug() << pkgName + " installed! ";
    }
    qDebug() << "successfully installed " << pkgList;
    this->close();
    proc->close();
    pkgList.clear();
    ui->pkg_list->clear();

    /*
     * second part: pip3 install
     */
    /*
     * TODO: check if all dependency installed again
     */
    emit allInstalled();
}

void DependencyInstaller::on_button_box_accepted()
{
    this->switchInstallGUI();
    //if rejected , go back to check GUI.
    connect(pwdDialog,&QInputDialog::rejected,this,&DependencyInstaller::switchCheckGUI);
    connect(pwdDialog,&QInputDialog::textValueSelected,this,&DependencyInstaller::installDependencies);
    pwdDialog->open();
}
