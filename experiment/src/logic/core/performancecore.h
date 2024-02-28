#ifndef SCRIPTCORE_H
#define SCRIPTCORE_H

#include "inc.h"
#include "logic/utils/subcore.h"

//!性能仿真功能模块
class PerformanceCore : public SubCore
{
    Q_OBJECT
public:
    explicit PerformanceCore(Core *core,
                        AppModel* appModel,
                        TaskProcess* pubProc,
                        TaskProcess* priProc);
public:
    /*!
     * \brief 检查用户是否配置成功
     * \return 用户是否配置成功
     */
    bool checkConfigured();

    /*!
     * \brief 检查脚本是否成功生成
     * \return 如果用户未配置，返回true；如果用户已配置，返回所有的脚本是否成功生成。
     */
    bool checkGenScript();

public slots:
    void clearConfig();
    void cleanScript();
    void genScript();
    void simulatePerformance();

signals:
    void simulatePerformanceStarted();
    void simulatePerformanceFinished();

    void logProgram(QString info, QString program);

};

#endif // SCRIPTCORE_H
