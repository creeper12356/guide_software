#ifndef SUBCORE_H
#define SUBCORE_H
#include "inc.h"
//Core子功能模块的接口
class SubCore : public QObject
{
    Q_OBJECT
public:
    explicit SubCore(Core *core = nullptr ,
                     AppModel* appModel = nullptr,
                     TaskProcess* pubProc = nullptr,
                     TaskProcess* priProc = nullptr
                     );
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
