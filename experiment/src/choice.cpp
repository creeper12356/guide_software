#include "choice.h"


void Choice::readFromJsonObject(const QJsonObject &jsonObject)
{
    architecture = jsonObject["architecture"].toString();
    set = jsonObject["set"].toString();
    auto programsJsonArray = jsonObject["programs"].toArray();
    for(auto it = programsJsonArray.begin();it != programsJsonArray.end();++it){
        programs.append((*it).toString());
    }
    test = jsonObject["test"].toString();
    threadNum = jsonObject["threadNum"].toInt();
}

QJsonObject Choice::toJsonObject()
{
    QJsonObject jsonObject;
    jsonObject.insert("architecture",architecture);
    jsonObject.insert("set",set);
    QJsonArray programsJsonArray;
    for(const QString& program : programs){
        programsJsonArray.append(program);
    }
    jsonObject.insert("programs",programsJsonArray);
    jsonObject.insert("test",test);
    jsonObject.insert("threadNum",threadNum);

    return jsonObject;
}

bool Choice::isConfigured() const
{
    return !programs.isEmpty();
}
