#ifndef CHOICE_H
#define CHOICE_H
#include "inc.h"
//用户选择的结构体
class Choice{
public:
    QString architecture = "";
    QString set = "";
    QStringList programs;
    QString test = "";
    int threadNum = 0;

    Choice(){}

    void readFromJsonObject(const QJsonObject& jsonObject);
    QJsonObject toJsonObject();
    bool isConfigured() const;
};


#endif // CHOICE_H
