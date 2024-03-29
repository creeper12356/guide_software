#include "taskmanager.h"

TaskProcess::TaskProcess(QObject *parent)
    :QProcess(parent)
{
    setProgram("bash");

    mEventLoop = new TaskEventLoop(this);
    connect(this,SIGNAL(finished(int)),mEventLoop,SLOT(quit()));

    setEnabled(true);
    connect(this,&TaskProcess::started,this,[this]() {
       emit stdinUpdated(QDir(workingDirectory()).absolutePath(),arguments()[1]);
    });
    connect(this,&TaskProcess::readyReadStandardOutput,this,[this]() {
        mCache = this->readAllStandardOutput();
        emit stdoutUpdated(QString::fromUtf8(mCache));
    });
    connect(this,&TaskProcess::readyReadStandardError,this,[this]() {
        mCache = this->readAllStandardError();
        emit stderrUpdated(QString::fromUtf8(mCache));
    });
}

void TaskProcess::setEnabled(bool flag)
{
    mEventLoop->setEnabled(flag);
    mEnabled = flag;
}

bool TaskProcess::isEnabled() const
{
    return mEnabled;
}

const QByteArray &TaskProcess::cache() const
{
    return mCache;
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

void TaskProcess::noBlockWaitForFinished(const QString &command)
{
    setArguments(QStringList() << "-c" << command);
    start();
    mEventLoop->exec();
}

void TaskProcess::blockWaitForFinished(const QString &command)
{
    setArguments(QStringList() << "-c" << command);
    start();
    waitForFinished(-1);
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
