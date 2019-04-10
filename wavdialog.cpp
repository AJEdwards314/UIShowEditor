#include "wavdialog.h"
#include "ui_wavdialog.h"
#include "wavtrack.h"
#include "portconfig.h"

#include <QAbstractButton>
#include <QDebug>

WAVDialog::WAVDialog(QWidget *parent, QString filename, int offset, QString port) :
    QDialog(parent),
    ui(new Ui::WAVDialog)
{
    parentTrack = (WAVTrack*) parent;
    ui->setupUi(this);
    ui->filenameLbl->setText(filename);
    ui->offsetBox->setText(QString::number(offset));
    ui->portCB->addItems(PortConfig::getInstance()->getPorts("AUD"));
    ui->portCB->setCurrentText(port);

}

WAVDialog::~WAVDialog()
{
    delete ui;
}

void WAVDialog::on_buttonBox_accepted()
{
    applyToParent();
}

void WAVDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    if(button->text() == "Apply")
        applyToParent();
}

void WAVDialog::applyToParent()
{
    parentTrack->apply(ui->offsetBox->text().toInt(), ui->portCB->currentText());
}
