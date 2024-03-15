#include "compatibility.h"

bool Compatibility::mInDocker;
QString Compatibility::mPythonInterpreter;
QString Compatibility::mPip;
QString Compatibility::mAptRequirementFile;
QString Compatibility::mPipRequirementFile;

void Compatibility::initialize()
{
    // 目前只支持Ubuntu16.04和Ubuntu20.04
    if(QSysInfo::productType() != "ubuntu") {
        qFatal("non-Ubuntu system detected.");
    }
    if(QSysInfo::productVersion() == "16.04") {
        qDebug() << "Ubuntu16.04 detected";
        mPythonInterpreter = "python";
        mPip = "pip";
        mAptRequirementFile = "config/requirements.txt";
        mPipRequirementFile = "config/py_requirements.txt";
    }
    else if(QSysInfo::productVersion() == "20.04"){
        qDebug() << "Ubuntu20.04 detected";
        mPythonInterpreter = "python3";
        mPip = "pip3";
        mAptRequirementFile = "config/20requirements.txt";
        mPipRequirementFile = "config/py_requirements.txt";
    }
    else {
        qFatal("Unsupported Ubuntu version.");
    }

    // init inDocker
    mInDocker = false;
    QFile cgroupFile("/proc/1/cgroup");
    if (cgroupFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString content = cgroupFile.readAll();
        cgroupFile.close();
        if (content.contains("/docker/")) {
            mInDocker = true;
        }
    }
}

bool Compatibility::isRunningInDocker()
{
    return mInDocker;
}

const QString &Compatibility::python()
{
    return mPythonInterpreter;
}

const QString &Compatibility::pip()
{
    return mPip;
}

const QString &Compatibility::aptRequirementFile()
{
    return mAptRequirementFile;
}

const QString Compatibility::pipRequirementFile()
{
    return mPipRequirementFile;
}
