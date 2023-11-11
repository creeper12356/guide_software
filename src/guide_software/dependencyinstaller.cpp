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
}

DependencyInstaller::~DependencyInstaller()
{
    delete ui;
}

QStringList DependencyInstaller::checkDependencies()
{
    this->show();
    QProcess* proc = new QProcess(this);
    proc->start("firefox",QStringList());
//    if(!proc->waitForStarted()){
//        qDebug() << "cannot start process.";
//        return QStringList();
//    }
//    if(!proc->waitForFinished(3000)){
//        qDebug() << "timeout";
//        return QStringList();
//    }
//    qDebug() << proc->readAllStandardOutput();
//    proc->close();
}
