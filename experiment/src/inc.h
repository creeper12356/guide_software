#ifndef INC_H
#define INC_H
//包含该项目所有Qt标准库，宏定义，类声明，枚举声明，命名空间
#include <QObject>
#include <QApplication>
#include <QProcess>

#include <QSize>

#include <QDialog>
#include <QInputDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QToolButton>
#include <QButtonGroup>
#include <QToolBar>
#include <QDockWidget>
#include <QPixmap>
#include <QLabel>
#include <QTextBrowser>
#include <QPlainTextEdit>
#include <QScrollArea>
#include <QScrollBar>

#include <QAction>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QFile>
#include <QDir>

#include <QKeyEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QEventLoop>
#include <QTimer>

#include <QDebug>
#define SECOND 1000
/*
 * 无阻塞等待
 * QProcess* process,
 * QString command,
 * QEventLoop* eventLoop
 */
#define noBlockWait(process,command,eventLoop) \
    process->setArguments(QStringList() << "-c" << command);\
    process->start();\
    eventLoop->exec();
/*
 * 阻塞等待
 * QProcess* process,
 * QString command
 */
#define blockWait(process,command) \
    process->setArguments(QStringList() << "-c" << command);\
    process->start();\
    process->waitForFinished(-1);

class Core;
class MainPage;
class DependencyInstaller;
class AptInstaller;
class PyLibInstaller;
class PasswordDialog;
class Choice;
class ChoiceGuide;
class ChoiceWidget;
class ImageDisplay;
class ConsoleDock;
class TaskProcess;
class TaskEventLoop;

class AppModel;

enum Exception{
    UserAbort
};

#endif // INC_H
