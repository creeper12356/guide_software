#include "taskmanager.h"

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
}

bool TaskProcess::isEnabled() const
{
    return mEnabled;
}

void TaskProcess::start(QIODevice::OpenMode mode)
{
    if(mEnabled){
        QProcess::start(mode);
    }
    else{
        qDebug() << "cannot start because process is disabled.";
        emit finished(0);
    }
}

TaskEventLoop::TaskEventLoop(QObject *parent)
    :QEventLoop(parent)
{
}

void TaskEventLoop::setEnabled(bool flag)
{
    mEnabled = flag;
}

int TaskEventLoop::exec(QEventLoop::ProcessEventsFlags flags)
{
    if(mEnabled){
        return QEventLoop::exec(flags);
    }
    else{
        qDebug() << "cannot start because event loop is disabled.";
        return 1;
    }
}
