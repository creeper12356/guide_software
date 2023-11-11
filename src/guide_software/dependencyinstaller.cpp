#include "inc.h"
#include "dependencyinstaller.h"
#include "ui_dependencyinstaller.h"
#include "core.h"
#include <QInputDialog>
DependencyInstaller::DependencyInstaller(Core *c, QWidget *parent) :
    QDialog(parent),
    core(c),
    ui(new Ui::DependencyInstaller)
{
    ui->setupUi(this);
    proc = new QProcess(this);
    proc->setProgram("bash");
    proc->setArguments(QStringList() << "-c" << "xargs apt list < ./requirements.txt");

    connect(proc,SIGNAL(finished(int)),this,SLOT(processDependencies()));
}

DependencyInstaller::~DependencyInstaller()
{
    delete ui;
}
void DependencyInstaller::checkDependencies()
{
    ui->textBrowser->clear();
    if(proc->state() != QProcess::NotRunning){
        qDebug() << "running";
        return ;
    }
    proc->start();
    qDebug() << "started.";
}

void DependencyInstaller::processDependencies()
{
    qDebug() << "finished.";
    QString res = QString(proc->readAllStandardOutput());
    ui->textBrowser->setText(res);
}
