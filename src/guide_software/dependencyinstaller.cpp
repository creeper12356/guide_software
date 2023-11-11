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
void DependencyInstaller::checkDependencies()
{
    static int count = 0;
    qDebug() << count;
    QProcess* proc = new QProcess(this);
    proc->setProgram("bash");
    proc->setArguments(QStringList() << "-c" << "apt list");
    proc->start();
    proc->waitForFinished(10000);
    qDebug() << "result == " << proc->readAll();
    proc->close();
    ++count;
}
