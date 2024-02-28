#include "heatmapcore.h"

#include "logic/core/core.h"
#include "logic/utils/taskmanager.h"
#include "logic/utils/compatibility.h"

HeatMapCore::HeatMapCore(Core *core, AppModel *appModel, TaskProcess *pubProc, TaskProcess *priProc)
    : SubCore(core , appModel , pubProc, priProc)
{
    connect(this,&HeatMapCore::logProgram, core,&Core::logProgram);
    connect(this,&HeatMapCore::genHeatMapStarted, core, &Core::longTaskStarted);
    connect(this,&HeatMapCore::genHeatMapFinished, core,&Core::longTaskFinished);
}

bool HeatMapCore::splitGem5Output(const QString &program)
{
    //准备输出文件夹
    QString mkdirCmd = "mkdir -p McPAT_input/%1";
    mkdirCmd = mkdirCmd.arg(program);
    mPriProc->blockWaitForFinished(mkdirCmd);

    //分割stats文件
    //python scripts/split.py gem5_output/{program}/stats.txt McPAT_input/
    QString splitCmd = Compatibility::python() + " scripts/split.py "
                                                 "gem5_output/%1/stats.txt "
                                                 "McPAT_input/";
    splitCmd = splitCmd.arg(program);
    mPubProc->blockWaitForFinished(splitCmd);
    if(mPubProc->cache() == "True\n"){
        //分割成功
        //in folder McPAT_input
        qDebug() << "split success";
        //在McPAT_input中创建文件夹，
        //将分割好的数据移动到该文件夹
        mPriProc->blockWaitForFinished(QString("cd McPAT_input ; "
                                               "mkdir %1 ; "
                                               "mv ./*.txt %1/ ; "
                                               "cd ..").arg(program));
        return true;
    }
    else{
        //分割失败
        //TODO: 删除所在文件夹
        return false;
    }
}

void HeatMapCore::genXml(const QString &program)
{
    //python scripts/generateXML.py McPAT_input/{program}/3.txt utils/template.xml McPAT_input/{program}/3.xml
    QString genXmlCmd = Compatibility::python() + " scripts/generateXML.py "
                                                  "McPAT_input/%1/3.txt "
                                                  "utils/template.xml "
                                                  "McPAT_input/%1/3.xml";
    genXmlCmd = genXmlCmd.arg(program);
    mPubProc->blockWaitForFinished(genXmlCmd);
}

void HeatMapCore::runMcpat(const QString &program)
{
    //输入的xml文件
    QString xmlFile = "McPAT_input/%1/3.xml";
    xmlFile = xmlFile.arg(program);

    QString mkdirCmd = "mkdir -p McPAT_output/%1";
    mkdirCmd = mkdirCmd.arg(program);
    mPriProc->blockWaitForFinished(mkdirCmd);
    QString mcpatCmd = "mcpat/mcpat "
                       "-infile %2 "
                       "-print_level 5 "
                       "> McPAT_output/%1/3.txt";
    mcpatCmd = mcpatCmd.arg(program,xmlFile);
    mPubProc->noBlockWaitForFinished(mcpatCmd);
}

void HeatMapCore::writePtrace(const QString &program)
{
    //准备输出文件夹
    QString mkdirCmd = "mkdir HotSpot_input";
    mPriProc->blockWaitForFinished(mkdirCmd);
    //调用python脚本
    //python scripts/writeptrace.py utils/ev6.flp McPAT_output/{program}/3.txt HotSpot_input/{program}.ptrace
    QString writePtraceCmd = Compatibility::python() + " scripts/writeptrace.py "
                             "utils/ev6.flp "
                             "McPAT_output/%1/3.txt "
                             "HotSpot_input/%1.ptrace";
    writePtraceCmd = writePtraceCmd.arg(program);
    mPubProc->blockWaitForFinished(writePtraceCmd);
}

void HeatMapCore::runHotspot(const QString &program)
{
    //创建HotSpot子文件夹
    QString mkdirCmd = "mkdir -p HotSpot_output/%1";
    mkdirCmd = mkdirCmd.arg(program);
    mPriProc->blockWaitForFinished(mkdirCmd);
    QString hotspotCmd = "./hotspot "
                         "-c ../utils/example.config "
                         "-f ../utils/ev6.flp "
                         "-p ../HotSpot_input/%1.ptrace "
                         "-materials_file ../utils/example.materials "
                         "-o ../HotSpot_output/%1/%1.ttrace "
                         "-steady_file ../HotSpot_output/%1/%1.steady "
                         "-model_type grid "
                         "-grid_steady_file ../HotSpot_output/%1/%1.grid.steady ";
    hotspotCmd = hotspotCmd.arg(program);
    mPubProc->setWorkingDirectory("HotSpot-master");
    mPubProc->noBlockWaitForFinished(hotspotCmd);
    mPubProc->setWorkingDirectory(".");
}

void HeatMapCore::drawHeatMap(const QString &program)
{
    //TODO: 检查输出文件夹./HeatMap

    //python scripts/flpdraw.py utils/ev6.flp HotSpot_output/{program}/{program}.grid.steady 0 {program} HeatMap/{program}
    QString heatMapCmd = Compatibility::python() + " scripts/flpdraw.py "
                         "utils/ev6.flp "
                         "HotSpot_output/%1/%1.grid.steady "
                         "0 "
                         "%1 "
                         "HeatMap/%1";
    heatMapCmd = heatMapCmd.arg(program);
    mPubProc->noBlockWaitForFinished(heatMapCmd);
}

void HeatMapCore::genHeatMap()
{
    emit log("开始生成温度图...");
    //准备输入文件夹
    if(QDir::current().exists("McPAT_input")){
        mPubProc->noBlockWaitForFinished("rm McPAT_input/* -rf");
    }
    else {
        QDir::current().mkdir("McPAT_input");
    }
    //仿真结果的所有文件夹列表
    QStringList resultPrograms = QDir("gem5_output").entryList(QDir::NoDotAndDotDot | QDir::Dirs);
    if(resultPrograms.empty()){
        emit warning("找不到可用的程序，请检查目录gem5_output/。");
        return ;
    }
    //检查关键模块
    if(!QDir("mcpat").exists()) {
        emit critical("找不到目录mcpat/。");
        return ;
    }
    if(!QDir("HotSpot-master").exists()) {
        emit critical("找不到目录HotSpot-master/。");
        return ;
    }

    //检查仿真是否成功
    //TODO : 检查性能仿真文件夹是否有5个文件

    emit genHeatMapStarted();

    //准备温度图文件夹
    mPriProc->blockWaitForFinished("mkdir HotSpot_output ; rm HotSpot_output/* -rf");
    mPriProc->blockWaitForFinished("mkdir HeatMap ; rm HeatMap/* -rf");

    //处理性能数据
    for(auto& program: resultPrograms){
        emit logProgram(program,"分割性能数据...");
        if(!splitGem5Output(program)){
            emit logProgram(program,"[FAIL]分割性能仿真结果失败。终止。");
            continue;
        }
        emit logProgram(program,"生成xml文件...");
        genXml(program);
        emit logProgram(program,"运行McPAT模块...");
        runMcpat(program);
        log("完成!");
        emit logProgram(program,"生成ptrace文件...");
        writePtrace(program);
        emit logProgram(program,"运行Hotspot模块...");
        runHotspot(program);
        log("完成!");
        emit logProgram(program,"生成温度图...");
        drawHeatMap(program);
        if(QFile(QString("HeatMap/%1.png").arg(program)).exists()) {
            //TODO : 温度图超链接
            emit logProgram(program,"[SUCCESS]温度图已成功生成(HeatMap/" + program + ".png)。 ");
        }
    }

    mPubProc->setEnabled(true);
    mPriProc->setEnabled(true);

    //删除除了HotSpot_output之外的所有中间文件夹
    mPriProc->blockWaitForFinished("rm McPAT_input McPAT_output HotSpot_input -rf");
    emit log("生成温度图完成。");
    emit genHeatMapFinished();
}

void HeatMapCore::probe(QString program, qreal probeX, qreal probeY)
{
    //TODO : 检查温度图是否生成
    //TODO : read 0.015 from elsewhere
    if(probeX < 0 || probeX > 0.015 || probeY < 0 || probeY > 0.015) {
        qDebug() << "overflow";
        return ;
    }

    QFile steadyFile(QString("HotSpot_output/%1/%1.grid.steady").arg(program));
    const int layer = 0;
    if(!steadyFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return ;
    }
    QTextStream ts(&steadyFile);
    QString line;
    QVector<qreal> temps;
    int count = 0;
    while(!ts.atEnd()) {
        line = ts.readLine();
        if(line[0] == 'L' && count <= layer) {
            ++count;
            continue;
        }
        if(line[0] == 'L' && count > layer) {
            break;
        }
        if(count - 1 == layer) {
            temps.append(QString(line.split(QRegExp("\\s+"))[1]).toDouble());
        }
    }
    qDebug() << "temps size : " << temps.size();
    steadyFile.close();
    const int length = 64; //sqrt(temps.size())
    int col = int(probeX / 0.015 * length);
    int row = int((0.015 - probeY) / 0.015 * length);
    int inx = row * length + col;
    qDebug() << "inx: " << inx;

    emit probeResult(temps[inx],probeX,probeY);
}

