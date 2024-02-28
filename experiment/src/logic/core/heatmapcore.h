#ifndef HEATMAPCORE_H
#define HEATMAPCORE_H
#include "inc.h"
#include "logic/utils/subcore.h"

//!生成温度图功能模块
class HeatMapCore : public SubCore
{
    Q_OBJECT
public:
    HeatMapCore(Core* core);
};

#endif // HEATMAPCORE_H
