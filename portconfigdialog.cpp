#include "portconfigdialog.h"
#include "ui_portconfigdialog.h"

PortConfigDialog::PortConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PortConfigDialog)
{
    ui->setupUi(this);
}

PortConfigDialog::~PortConfigDialog()
{
    delete ui;
}
