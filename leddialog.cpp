#include "leddialog.h"
#include "ui_leddialog.h"
#include "ledtrack.h"

#include <QAbstractButton>
#include <QDebug>

LEDDialog::LEDDialog(QWidget *parent, QString filename, QString name, int offset, QString port, QString colorName) :
    QDialog(parent),
    ui(new Ui::LEDDialog)
{
    parentTrack = (LEDTrack*) parent;
    ui->setupUi(this);
    ui->filenameLbl->setText(filename);
    ui->nameBox->setText(name);
    ui->offsetBox->setText(QString::number(offset));
    ui->portBox->setText(port);
    ui->colorBox->setText(colorName);

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
    parentTrack->apply(ui->nameBox->text(), ui->offsetBox->text().toInt(), ui->portBox->text(), ui->colorBox->text());
}
