#include "wavdialog.h"
#include "ui_wavdialog.h"
#include "wavshowbarwidget.h"

#include <QAbstractButton>
#include <QDebug>

WAVDialog::WAVDialog(QWidget *parent, QString filename, int offset, QString port) :
    QDialog(parent),
    ui(new Ui::WAVDialog)
{
    parentShowBar = (WAVShowBarWidget*) parent;
    ui->setupUi(this);
    ui->filenameLbl->setText(filename);
    ui->offsetBox->setText(QString::number(offset));
    ui->portBox->setText(port);

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
    parentShowBar->apply(ui->offsetBox->text().toInt(), ui->portBox->text());
}
