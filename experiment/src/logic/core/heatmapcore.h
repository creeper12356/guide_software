#ifndef HEATMAPCORE_H
#define HEATMAPCORE_H
#include "inc.h"
#include "logic/utils/subcore.h"

//!生成温度图功能模块
class HeatMapCore : public SubCore
{
    Q_OBJECT
public:

    /*!
     * \brief HeatMapCore构造函数
     * \param core 核心类指针
     * \param appModel 数据模型指针
     * \param pubProc 公有进程指针
     * \param priProc 私有进程指针
     */
    HeatMapCore(Core* core ,AppModel* appModel , TaskProcess* pubProc, TaskProcess* priProc);

public :

    /*!
     * \brief 分割gem5输出的性能数据
     * \param program 基准程序名
     * \return 是否分割成功
     */
    bool splitGem5Output(const QString& program);

    /*!
     * \brief 将分割后的性能数据转换为xml文件
     * \param program 基准程序名
     */
    void genXml(const QString& program);

    /*!
     * \brief 运行McPAT模块
     * \param program 基准程序名
     */
    void runMcpat(const QString &program);

    /*!
     * \brief 将功耗数据转换为ptrace文件
     * \param program 基准程序名
     */
    void writePtrace(const QString& program);

    /*!
     * \brief 运行HotSpot模块，生成grid.steady文件
     * \param program 基准程序名
     */
    void runHotspot(const QString& program);

    /*!
     * \brief 调用脚本绘制温度图
     * \param program 基准程序名
     */
    void drawHeatMap(const QString& program);

public slots:
    //!生成温度图
    void genHeatMap();
    //!探针函数
    void probe(QString program , qreal probeX , qreal probeY);
signals:
    void genHeatMapStarted();
    void genHeatMapFinished();
    void probeResult(qreal temperature , qreal probeX, qreal probeY);

    void logProgram(QString info , QString program);
};

#endif // HEATMAPCORE_H
