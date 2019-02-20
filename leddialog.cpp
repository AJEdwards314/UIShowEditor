#include "leddialog.h"
#include "ui_leddialog.h"
#include "ledtrack.h"
#include "portconfig.h"

#include <QAbstractButton>
#include <QDebug>

LEDDialog::LEDDialog(QWidget *parent, QString filename, QString name, int offset, QString port) :
    QDialog(parent),
    ui(new Ui::LEDDialog)
{
    parentTrack = (LEDTrack*) parent;
    ui->setupUi(this);
    ui->filenameLbl->setText(filename);
    ui->nameBox->setText(name);
    ui->offsetBox->setText(QString::number(offset));
    ui->portCB->addItems(PortConfig::getInstance()->getPorts("DOUT"));
    ui->portCB->setCurrentText(port);
    //ui->colorBox->setText(colorName);

}

LEDDialog::~LEDDialog()
{
    delete ui;
}

void LEDDialog::on_buttonBox_accepted()
{
    applyToParent();
}

void LEDDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    if(button->text() == "Apply")
        applyToParent();
}

void LEDDialog::applyToParent()
{
    QString port = ui->portCB->currentText();
    parentTrack->apply(ui->nameBox->text(), ui->offsetBox->text().toInt(), port);
}
