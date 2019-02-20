#include "inputportconfigwidget.h"
#include "ui_inputportconfigwidget.h"

#include "portconsts.h"

InputPortConfigWidget::InputPortConfigWidget(PortConfig::InConfig ** inConfig, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InputPortConfigWidget)
{
    ui->setupUi(this);
    ui->typeCB->addItems(PortConsts::inStrings);
    this->inConfig = inConfig;
}

InputPortConfigWidget::~InputPortConfigWidget()
{
    delete ui;
}

void InputPortConfigWidget::showEvent(QShowEvent * event)
{
    ui->portNumLBL->setText(QString::number((*inConfig)->portNum));
    ui->typeCB->setCurrentText((*inConfig)->type);
}

void InputPortConfigWidget::accepted()
{
    (*inConfig)->type = ui->typeCB->currentText();
    (*inConfig)->portNum = ui->portNumLBL->text().toInt();
}
