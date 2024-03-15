#pragma once
#include "inc.h"
// source code refs: https://github.com/mapeditor/tiled

//! 终端窗口
class ConsoleDock : public QDockWidget
{
    Q_OBJECT
public:
    explicit ConsoleDock(QWidget *parent = nullptr);
    ~ConsoleDock() override;

    void clear();

public slots:
    void appendStdin(QString dir , QString info);
    void appendStdout(QString info);
    void appendStderr(QString info);

private:
    QPlainTextEdit *mPlainTextEdit = nullptr;
};
