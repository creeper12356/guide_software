#pragma once
#ifndef CHOICEGUIDE_H
#define CHOICEGUIDE_H
#include "inc.h"

namespace Ui {
class ChoiceGuide;
}

class ChoiceGuide : public QMainWindow
{
    Q_OBJECT

public:
    explicit ChoiceGuide(QWidget *parent = nullptr);
    ~ChoiceGuide();

private:
    Ui::ChoiceGuide *ui;
};

#endif // CHOICEGUIDE_H
