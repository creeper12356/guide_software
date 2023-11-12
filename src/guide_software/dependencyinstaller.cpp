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
    proc = new QProcess(this);
    proc->setProgram("bash");
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

void DependencyInstaller::installDependencies(const QString& pwd)
{
    //TODO: if user enters wrong password?
    qDebug() << "start installing...";
    /*
     * this command allows sudo apt install without
     * manually input password in terminal
     */
    QString installCmd =
            "echo " + pwd +
            " | sudo -S apt install -y ";
    for(auto pkgName:pkgList){
        installCmd += pkgName + " ";
    }
    proc->setArguments(QStringList() << "-c" << installCmd);
    proc->start();
    proc->waitForStarted();
    proc->waitForFinished();
    qDebug() << "successfully installed " << pkgList;
    this->close();
    pkgList.clear();
    ui->pkg_list->clear();
    emit allInstalled();
}

void DependencyInstaller::on_button_box_accepted()
{
    QInputDialog pwdDialog(this);
    pwdDialog.setLabelText("我们需要您提供用户的密码：");
    pwdDialog.setTextEchoMode(QLineEdit::Password);
    connect(&pwdDialog,&QInputDialog::textValueSelected,[&pwdDialog,this](const QString& pwd){
        qDebug() << "your pwd is " << pwd;
        pwdDialog.close();
        installDependencies(pwd);
    });
    pwdDialog.exec();
}
