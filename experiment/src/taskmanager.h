#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include "inc.h"

class TaskProcess : public QProcess
{
    Q_OBJECT
public:
    explicit TaskProcess(QObject* parent = 0);
    void setEnabled(bool flag);
    bool isEnabled() const;
    const QByteArray& cache() const;
    void start(OpenMode mode = ReadWrite);
signals:
    void stdinUpdated(QString dir, QString info);
    void stdoutUpdated(QString info);
    void stderrUpdated(QString info);
private:
    bool mEnabled = true;
    QByteArray mCache;
};

class TaskEventLoop: public QEventLoop
{
    Q_OBJECT
public :
    explicit TaskEventLoop(QObject* parent = 0);
    void setEnabled(bool flag);
    int exec(ProcessEventsFlags flags=AllEvents);
private:
    bool mEnabled = true;
};

class TaskManager : public QObject
{
    Q_OBJECT
public:
    explicit TaskManager(QObject *parent = 0);

signals:

public slots:

private:
    QProcess* mProcess = nullptr;
};

#endif // TASKMANAGER_H
