#include "motordialog.h"
#include "ui_motordialog.h"
#include "motortrack.h"
#include "portconfig.h"

#include <QAbstractButton>
#include <QDebug>

MotorDialog::MotorDialog(QWidget *parent, QString filename, QString name, int offset, QString port) :
    QDialog(parent),
    ui(new Ui::MotorDialog)
{
    parentTrack = (MotorTrack*) parent;
    ui->setupUi(this);
    ui->filenameLbl->setText(filename);
    ui->nameBox->setText(name);
    ui->offsetBox->setText(QString::number(offset));
    ui->portCB->addItems(PortConfig::getInstance()->getPorts("SRV"));
    ui->portCB->setCurrentText(port);

}

MotorDialog::~MotorDialog()
{
    delete ui;
}

void MotorDialog::on_buttonBox_accepted()
{
    applyToParent();
}

void MotorDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    if(button->text() == "Apply")
        applyToParent();
}

void MotorDialog::applyToParent()
{
    QString port = ui->portCB->currentText();
    parentTrack->apply(ui->nameBox->text(), ui->offsetBox->text().toInt(), port);
}
