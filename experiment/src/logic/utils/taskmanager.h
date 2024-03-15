#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include "inc.h"

// 该文件存放所有与任务(Task)相关的类和函数


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



/*!
 * \brief 任务进程类
 * \details 任务进程类，对QProcess的功能进行了拓展：
 *          1. 启用和禁用。使用setEnabled函数对进程启用和禁用，禁用的进程将无法启动。
 *          2. 阻塞和非阻塞等待。拓展了waitForFinished函数，支持阻塞和非阻塞等待进程结束。
 *          3. 信息更新的接口。通过信号stdinUpdated, stdoutUpdated, stderrUpdated，
 *          可以与外部类进行松耦合通信。
 */
class TaskProcess : public QProcess
{
    Q_OBJECT
public:
    explicit TaskProcess(QObject* parent = 0);

    void setEnabled(bool flag);
    bool isEnabled() const;
    const QByteArray& cache() const;
    void start(OpenMode mode = ReadWrite);

    void noBlockWaitForFinished(const QString& command);
    void blockWaitForFinished(const QString& command);
signals:
    void stdinUpdated(QString dir, QString info);
    void stdoutUpdated(QString info);
    void stderrUpdated(QString info);



private:
    bool mEnabled = true;

    //! 缓存，最近一次readAll操作的字节流
    QByteArray mCache;
    //! 内置的事件循环，用于实现非阻塞等待
    TaskEventLoop* mEventLoop;
};


#endif // TASKMANAGER_H
