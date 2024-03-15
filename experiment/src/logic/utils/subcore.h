#ifndef SUBCORE_H
#define SUBCORE_H
#include "inc.h"

/*!
 * \brief 子核心模块接口类
 */
class SubCore : public QObject
{
    Q_OBJECT
public:

    /*!
     * \brief 子核心构造函数
     * \param core 指向核心类的指针，同时作为对象树中子核心的父对象
     * \param appModel 指向数据模型的指针
     * \param pubProc 指向公有进程的指针
     * \param priProc 指向私有进程的指针
     */
    explicit SubCore(Core *core = nullptr ,
                     AppModel* appModel = nullptr,
                     TaskProcess* pubProc = nullptr,
                     TaskProcess* priProc = nullptr);
signals:
    void log(QString info);
    void warning(QString info);
    void critical(QString info);

protected:
    AppModel* mAppModel;
    TaskProcess* mPubProc;
    TaskProcess* mPriProc;
};

#endif // SUBCORE_H
