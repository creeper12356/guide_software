#ifndef CHOICEGUIDE_H
#define CHOICEGUIDE_H
#include "inc.h"
#include "choice.h"
namespace Ui {
class ChoiceGuide;
}

class ChoiceGuide : public QMainWindow
{
    Q_OBJECT
private:
    const QStringList infos =
    {
        "请选择架构和程序集",
        "请选择程序", "请选择测试集，并输入线程数"
    };
private:
    //使用QJsonObject表示的所有程序选择分支信息
    QJsonObject programInfo;
    //使用QJsonObject表示的测试集列表，测试集名称为所引
    QJsonObject testInfo;
    //用户选择
    Choice _userChoice;

    //选择架构的按钮组
    QButtonGroup* archGroup = nullptr;
    //选择程序集的按钮组
    QButtonGroup* setGroup = nullptr;

public:
    ChoiceGuide();
    ~ChoiceGuide();
public:
    //getters
    //返回指向用户选择的常量指针
    const Choice *userChoice() const;

protected:
    //每次打开时的配置
    void showEvent(QShowEvent *event) override;
private:
    //初始化函数，在构造函数中调用
    void initArchitectButtons();
    void initSetButtons();
    void initTestList();

    //从json文件中加载选择分支信息
    void loadChoices();
    //清除所有程序集选择
    inline void clearSetChoice();
    //清除所有程序选择
    inline void clearProgramChoice();
    //更新完成按钮的状态
    void refreshFinishState();

private slots:
    /* jump back and force */
    void on_next_button_clicked();
    void on_prev_button_clicked();
    /* cancel and finish */
    void on_cancel_button_clicked();
    void on_finish_button_clicked();
    /* update states of buttons ans page index changes */
    void on_stacked_widget_currentChanged(int index);

    void architectChosenSlot(QString name);
    void setChosenSlot(QString name);
    void programChosenSlot();
    void testChosenSlot(QString name);
    void threadNumSetSlot(int threadNum);
    //全选或全不选
    void selectAllPrograms(bool flag);
private:
    Ui::ChoiceGuide *ui;
signals:
    //配置完成时，该信号发送给主页面
    void configureFinished(const Choice* userChoice);
};

#endif // CHOICEGUIDE_H
