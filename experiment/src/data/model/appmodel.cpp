#include "appmodel.h"

#include "data/utils/choice.h"
#include "ui/window/mainpage.h"

AppModel::AppModel(MainPage *mainPage)
    : QObject()
{
    mUserChoice = new Choice;
    mMainPage = mainPage;
    connect(this,&AppModel::userChoiceChanged,mMainPage,&MainPage::updateUserChoice);
    connect(mMainPage,&MainPage::configureFinished,this,&AppModel::setUserChoice);

    readFromFile("config/config.json");
}

AppModel::~AppModel()
{
    writeToFile("config/config.json");
    delete mUserChoice;
}

bool AppModel::readFromFile(const QString &fileName)
{
    QFile reader(fileName);
    reader.open(QIODevice::ReadOnly);
    if(!reader.isOpen()){
        return false;
    }
    QJsonObject configJsonObject = QJsonDocument::fromJson(reader.readAll()).object();
    reader.close();

    mUserChoice->readFromJsonObject(configJsonObject["userChoice"].toObject());
    emit userChoiceChanged(mUserChoice);
    return true;
}

void AppModel::writeToFile(const QString &fileName)
{
    QJsonObject configJsonObject;
    configJsonObject.insert("userChoice",mUserChoice->toJsonObject());

    QFile writer(fileName);
    writer.open(QIODevice::WriteOnly);
    writer.write(QJsonDocument(configJsonObject).toJson());
    writer.close();
}

const Choice *AppModel::userChoice() const
{
    return mUserChoice;
}

void AppModel::clearUserChoiceAndNotify()
{
    mUserChoice->programs.clear();
    emit userChoiceChanged(mUserChoice);
}

void AppModel::setUserChoice(const Choice *userChoice)
{
    *mUserChoice = *userChoice;
}

void AppModel::setUserChoiceAndNotify(const Choice *userChoice)
{
    *mUserChoice = *userChoice;
    emit userChoiceChanged(mUserChoice);
}

