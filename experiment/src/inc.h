#ifndef INC_H
#define INC_H
//包含该项目所有Qt标准库，宏定义，类声明，枚举声明，命名空间
#include <QObject>
#include <QApplication>
#include <QCoreApplication>
#include <QProcess>

#include <QSize>

#include <QDialog>
#include <QInputDialog>
#include <QMainWindow>
#include <QLineEdit>
#include <QMessageBox>
#include <QMenu>
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

#include <QGraphicsView>

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

class Core;
class MainPage;
class AppModel;

class DependencyInstaller;
class AptInstaller;
class PyLibInstaller;
class PasswordDialog;

class Choice;
class ChoiceGuide;
class ChoiceWidget;
class ProbeWidget;
class HeatMapViewer;
class ConsoleDock;
class LogDock;
class TaskProcess;
class Compatibility;

class PerformanceCore;
class HeatMapCore;
#endif // INC_H
