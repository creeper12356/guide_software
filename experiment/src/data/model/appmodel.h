#ifndef APPMODEL_H
#define APPMODEL_H
#include "inc.h"

/*!
 * \brief 数据模型类
 * \details 数据模型类(AppModel)主要用于软件数据的读取、写入、维护，与MainPage通过信号松耦合连接。
 *          所有关于软件文件读写的操作函数，都应该位于AppModel类中。
 */
class AppModel : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief AppModel构造函数
     * \param mainPage 与AppModel通信的主页面指针
     */
    explicit AppModel(MainPage* mainPage);
    ~AppModel();

    bool readFromConfigFile(const QString& configFileName);
    void writeToConfigFile(const QString& configFileName);

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
