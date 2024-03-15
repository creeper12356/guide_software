#ifndef LOGDOCK_H
#define LOGDOCK_H
#include "inc.h"

//!日志窗口
class LogDock : public QDockWidget
{
public:
    explicit LogDock(QWidget* parent = nullptr);
    ~LogDock();

    QTextBrowser* logBrowser();
    void clear();
    void append(const QString& text);

private:
    QTextBrowser* mLogBrowser;
};

#endif // LOGDOCK_H
