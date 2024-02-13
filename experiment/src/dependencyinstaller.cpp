#include "dependencyinstaller.h"
#include "passworddialog.h"
#include "ui_dependencyinstaller.h"
#include "compatibility.h"

DependencyInstaller::DependencyInstaller()
   : QDialog(nullptr)
   , ui(new Ui::DependencyInstaller)
{
    ui->setupUi(this);

    mProcess = new QProcess(this);
    mProcess->setProgram("bash");

    mPasswordDialog = new PasswordDialog(nullptr);
    mEventLoop = new QEventLoop(this);

    connect(this,&DependencyInstaller::installProcess,
            ui->progress_bar,&QProgressBar::setValue);

    connect(mProcess,SIGNAL(finished(int)),
            mEventLoop,SLOT(quit()));

    connect(mPasswordDialog,&PasswordDialog::finished,mEventLoop,&QEventLoop::quit);

    //Qt机制，信号发出时，按照槽函数的连接顺序执行槽，用户点击“Yes”时，
    //先调用setAccepted，再退出事件循环
    connect(ui->button_box,&QDialogButtonBox::accepted,
            this,&DependencyInstaller::setAccepted);
    connect(ui->button_box,&QDialogButtonBox::accepted,
            mEventLoop,&QEventLoop::quit);

    //用户点击“No”
    connect(ui->button_box,&QDialogButtonBox::rejected,
            mEventLoop,&QEventLoop::quit);
    connect(ui->button_box,&QDialogButtonBox::rejected,
            this, &DependencyInstaller::reject);
}

DependencyInstaller::~DependencyInstaller()
{
    delete mPasswordDialog;
    delete ui;
}
bool DependencyInstaller::checkDependencies()
{
    if(mProcess->state() != QProcess::NotRunning){
        qWarning() << "The process is busy now.";
        return true;
    }

    mProcess->setArguments(QStringList() << "-c" << getNotInstalledCommand());
    mProcess->start();
    mEventLoop->exec();
    mDependencyList.clear();

    QTextStream stream(mProcess);
    while(!stream.atEnd()){
        mDependencyList.append(stream.readLine());
    }

    if(!mDependencyList.empty()){
        //存在未满足依赖
        showUnmetDependencies(mDependencyList);
    }
    mProcess->close();
    return mDependencyList.empty();
}
void DependencyInstaller::installDependencies(QString pwd)
{
    if(!isSuperUser()){
        pwd = "";
    }
    if(mProcess->state() != QProcess::NotRunning){
        qWarning() << "The process is busy now.";
        return ;
    }
    qDebug() << "start installing...";
    //update
    if(!updateCommand().isEmpty()){
        mProcess->setArguments(QStringList() << "-c" << updateCommand().arg(pwd));
        mProcess->start();
        mEventLoop->exec();
        qDebug() << "update finished.";
    }
    int pkgCount = mDependencyList.count();
    int installCount = 0;
    for(auto pkgName:mDependencyList){
        mProcess->setArguments(QStringList() << "-c" << installCommand().arg(pwd,pkgName));
        mProcess->start();
        mEventLoop->exec();
        ++installCount;
        emit installProcess(100.0 * installCount / pkgCount);
        qDebug() << pkgName + " installed! ";
    }
    qDebug() << "successfully installed " << mDependencyList;
    mProcess->close();
    mDependencyList.clear();
}

bool DependencyInstaller::checkAndInstall()
{
    if(checkDependencies()){
        return true;
    }
    while(true){
        //pre-set flags
        mAccepted = false;

        this->show();
        mEventLoop->exec();
        if(!mAccepted){
            return false;
        }
        //用户接受安装

        if(isSuperUser()){
            //等待用户输入密码
            mPasswordDialog->show();
            mEventLoop->exec();
            if(mPasswordDialog->result() == QDialog::Rejected){
                continue;
            }
        }
        this->switchInstallGUI();
        installDependencies(mPasswordDialog->textValue());

        //再次检查依赖是否安装
        if(checkDependencies()){
            this->close();
            return true;
        }
    }
}

void DependencyInstaller::switchCheckGUI()
{
    ui->hint_label->setText(hintText());
    ui->stackedWidget->setCurrentIndex(0);
}

void DependencyInstaller::switchInstallGUI()
{
    ui->hint_label->setText("正在安装中，这可能需要一点时间...");
    ui->progress_bar->setValue(0);
    ui->stackedWidget->setCurrentIndex(1);
}


void DependencyInstaller::showUnmetDependencies(const QStringList& list)
{
    this->switchCheckGUI();
    ui->pkg_list->clear();
    ui->pkg_list->addItems(list);
}

void DependencyInstaller::setAccepted()
{
    mAccepted = true;
}
AptInstaller::AptInstaller()
    : DependencyInstaller()
{
}

QString AptInstaller::updateCommand()
{
    return "echo %1 | sudo -S apt update";
}

QString AptInstaller::getNotInstalledCommand()
{
    return QString(
           "xargs apt list --installed < %1 "
           "| tail -n +2 "
           "| cut -f 1 -d / "
           "| sort "
           "> config/installed.txt "
           "&& "
           "comm config/installed.txt %1 -13 "
           "&& "
           "rm config/installed.txt ").arg(Compatibility::aptRequirementFile());
}

QString AptInstaller::installCommand()
{
    return "echo %1"
           " | sudo -S apt install %2 -y "
           "2> /dev/null";
}

bool AptInstaller::isSuperUser()
{
    return true;
}

QString AptInstaller::hintText()
{
    return "检测到您的电脑上没有安装下列前置软件包:";
}



PyLibInstaller::PyLibInstaller()
    : DependencyInstaller()
{
}

QString PyLibInstaller::updateCommand()
{
    return "";
}

QString PyLibInstaller::getNotInstalledCommand()
{
    return QString(
           "%1 list | tail -n +3 | cut -f 1 -d ' ' | sort > config/py_installed.txt "
           "&& "
           "comm config/py_installed.txt %2 -13 "
           "&& "
           "rm config/py_installed.txt ").arg(Compatibility::pip() , Compatibility::pipRequirementFile());
}

QString PyLibInstaller::installCommand()
{
    return "%1" + Compatibility::pip() + " install %2 "
           "2> /dev/null";
}

bool PyLibInstaller::isSuperUser()
{
    return false;
}

QString PyLibInstaller::hintText()
{
    return "检测到您的电脑上没有安装下列Python库:";
}
