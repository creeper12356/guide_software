#ifndef COMPATIBILITY_H
#define COMPATIBILITY_H
#include "inc.h"


/*!
 * \brief 兼容类
 * \details 兼容类，主要用于处理软件环境的兼容性问题。
 *          该类为静态类，所有的函数和成员都是静态(static)的。
 *          目前完全支持Ubuntu16.04,部分支持Ubuntu20.04(gem5不可用)。
 */
class Compatibility
{
public:
    /*!
     * \brief 初始化函数
     * \details 识别运行环境，进行初始化操作。
     */
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
