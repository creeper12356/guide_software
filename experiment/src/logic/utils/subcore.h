#ifndef SUBCORE_H
#define SUBCORE_H
#include "inc.h"

/*!
 * \brief 子核心模块接口类
 * \details 子核心模块接口类，提供基本的接口与Core通信。
 *          为软件新增子功能模块，可以继承SubCore类，在派生类中完成子功能的函数。
 * \sa Core
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
    // 与Core相同的信号接口
    void log(QString info);
    void warning(QString info);
    void critical(QString info);

protected:
    AppModel* mAppModel;
    TaskProcess* mPubProc;
    TaskProcess* mPriProc;
};

#endif // SUBCORE_H
