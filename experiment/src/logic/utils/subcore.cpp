#include "subcore.h"
#include "logic/core/core.h"

SubCore::SubCore(Core *core, AppModel *appModel, TaskProcess *pubProc, TaskProcess *priProc)
    : QObject(core)
    , mAppModel(appModel)
    , mPubProc(pubProc)
    , mPriProc(priProc)
{
    connect(this,&SubCore::log,core,&Core::log);
    connect(this,&SubCore::warning,core,&Core::warning);
    connect(this,&SubCore::critical,core,&Core::critical);
}
