#include "motordialog.h"
#include "ui_motordialog.h"
#include "motortrack.h"

#include <QAbstractButton>
#include <QDebug>

MotorDialog::MotorDialog(QWidget *parent, QString filename, QString name, int offset, QString port, int maxVal, int minVal, int defVal, bool reverse) :
    QDialog(parent),
    ui(new Ui::MotorDialog)
{
    parentTrack = (MotorTrack*) parent;
    ui->setupUi(this);
    ui->filenameLbl->setText(filename);
    ui->nameBox->setText(name);
    ui->offsetBox->setText(QString::number(offset));
    ui->portBox->setText(port);
    ui->maxBox->setText(QString::number(maxVal));
    ui->minBox->setText(QString::number(minVal));
    ui->defaultBox->setText(QString::number(defVal));
    ui->reverseBox->setCheckState(reverse ? Qt::Checked : Qt::Unchecked);

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
    parentTrack->apply(ui->nameBox->text(), ui->offsetBox->text().toInt(), ui->portBox->text(), ui->maxBox->text().toInt(), ui->minBox->text().toInt(), ui->defaultBox->text().toInt(), ui->reverseBox->checkState() == Qt::Checked);
}
