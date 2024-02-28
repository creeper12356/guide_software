#ifndef COMPATIBILITY_H
#define COMPATIBILITY_H
#include "inc.h"

//兼容类，用于处理环境的兼容性问题
class Compatibility
{
public:
    static void initialize();
public:
    static bool isRunningInDocker();
    static const QString& python();
    static const QString& pip();
    static const QString& aptRequirementFile();
    static const QString pipRequirementFile();

private:
    static bool mInDocker;
    static QString mPythonInterpreter;
    static QString mPip;
    static QString mAptRequirementFile;
    static QString mPipRequirementFile;
};

#endif // COMPATIBILITY_H
