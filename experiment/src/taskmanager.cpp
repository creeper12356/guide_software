#include "taskmanager.h"
#include <unistd.h>
#include <signal.h>
TaskManager::TaskManager(QObject *parent) : QObject(parent)
{
}


TaskProcess::TaskProcess(QObject *parent)
    :QProcess(parent)
{
    setEnabled(true);
}

void TaskProcess::setEnabled(bool flag)
{
    mEnabled = flag;
    if(mEnabled){
        qDebug() << "TaskProcess: enabled.";
        disconnect(this,&TaskProcess::started,this,&TaskProcess::kill);
        connect(this,&TaskProcess::started,this,[this](){
            qDebug() << "bash pid:" << this->pid();
            qDebug() << "before setpgid: " ;
            qDebug() << "bash pgid:" << getpgid(this->pid());
            pid_t pid = this->pid();
            qDebug() << "pid: " << pid;
            setpgid(pid,pid);
//            qDebug() << "after setpgid: ";
//            qDebug() << "bash pgid:" << getpgid(this->pid());
        });
    }
    else{
        qDebug() << "TaskProcess: disabled.";
        connect(this,&TaskProcess::started,this,&TaskProcess::kill);
    }
}

bool TaskProcess::isEnabled() const
{
    return mEnabled;
}
