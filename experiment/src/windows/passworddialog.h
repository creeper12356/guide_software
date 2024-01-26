#ifndef PASSWORDDIALOG_H
#define PASSWORDDIALOG_H

#include "inc.h"
class PasswordDialog : public QInputDialog
{
public:
    explicit PasswordDialog(QWidget* parent = 0);
};

#endif // PASSWORDDIALOG_H
