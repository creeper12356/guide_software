#include "taskmanager.h"
TaskManager::TaskManager(QObject *parent) : QObject(parent)
{

}


TaskProcess::TaskProcess(QObject *parent)
    :QProcess(parent)
{
}

void TaskProcess::setEnabled(bool flag)
{
    mEnabled = flag;
    if(mEnabled){
        qDebug() << "TaskProcess: enabled.";
        disconnect(this,&TaskProcess::started,this,&TaskProcess::kill);
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
