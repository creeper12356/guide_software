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
    void start(OpenMode mode = ReadWrite);
private:
    bool mEnabled = true;
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
