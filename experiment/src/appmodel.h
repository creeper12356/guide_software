#ifndef APPMODEL_H
#define APPMODEL_H
#include "inc.h"
class AppModel : public QObject
{
    Q_OBJECT
public:
    explicit AppModel(MainPage* mainPage);
    ~AppModel();

    bool readFromFile(const QString& fileName);
    void writeToFile(const QString& fileName);

public:
    const Choice* userChoice() const;
    void clearUserChoiceAndNotify();
    void setUserChoice(const Choice* userChoice);
    void setUserChoiceAndNotify(const Choice* userChoice);
signals:
    void userChoiceChanged(const Choice* userChoice);
private:
    Choice* mUserChoice;
    MainPage* mMainPage;

};

#endif // APPMODEL_H
