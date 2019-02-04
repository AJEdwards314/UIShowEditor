#include "porttestdialog.h"
#include "ui_porttestdialog.h"

PortTestDialog::PortTestDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PortTestDialog)
{
    ui->setupUi(this);
}

PortTestDialog::~PortTestDialog()
{
    delete ui;
}
