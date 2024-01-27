#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog() :
    QDialog(nullptr),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
