#include "passworddialog.h"

PasswordDialog::PasswordDialog(QWidget *parent)
    : QInputDialog(parent)
{
    setLabelText("我们需要您提供用户的密码：");
    setTextEchoMode(QLineEdit::Password);
    setModal(true);
}

