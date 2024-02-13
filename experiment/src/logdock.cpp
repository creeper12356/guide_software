#include "logdock.h"

LogDock::LogDock(QWidget *parent)
    : QDockWidget(parent)
{
    setObjectName("logDock");
    mLogBrowser = new QTextBrowser(this);
    setWidget(mLogBrowser);
    setWindowTitle("日志");
}

LogDock::~LogDock()
{

}

QTextBrowser *LogDock::logBrowser()
{
    return mLogBrowser;
}

void LogDock::clear()
{
    mLogBrowser->clear();
}

void LogDock::append(const QString &text)
{
    mLogBrowser->append(text);
}

