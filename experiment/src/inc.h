#pragma once
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
#include <QButtonGroup>
#include <QToolBar>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QFile>
#include <QDir>

#include <QKeyEvent>
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
    proc->start();\
    eventLoop->exec();
class Core;
class MainPage;
class DependencyInstaller;
class PyLibInstaller;
class ChoiceGuide;
struct Choice;


#endif // INC_H