#include "outputportconfigwidget.h"
#include "ui_outputportconfigwidget.h"

#include "portconsts.h"

OutputPortConfigWidget::OutputPortConfigWidget(PortConfig::OutConfigBase ** outConfig, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OutputPortConfigWidget)
{
    ui->setupUi(this);
    ui->typeCB->addItems(PortConsts::outStrings);
    this->outConfig = outConfig;
    connect(ui->typeCB, &QComboBox::currentTextChanged, this, &OutputPortConfigWidget::cbChanged);
}


OutputPortConfigWidget::~OutputPortConfigWidget()
{
    delete ui;
}

void OutputPortConfigWidget::showEvent(QShowEvent * event) {
    ui->portNumLBL->setText(QString::number((*outConfig)->portNum));
    ui->typeCB->setCurrentText((*outConfig)->type);
    if((*outConfig)->type == "SRV") {
        ui->colorF->setEnabled(false);
        ui->minValF->setEnabled(true);
        ui->midValF->setEnabled(true);
        ui->maxValF->setEnabled(true);
        ui->reverseBox->setEnabled(true);

        PortConfig::SRVConfig * srvConfig = (PortConfig::SRVConfig *)*outConfig;
        ui->minValF->setText(QString::number(srvConfig->minVal));
        ui->midValF->setText(QString::number(srvConfig->midVal));
        ui->maxValF->setText(QString::number(srvConfig->maxVal));
        ui->reverseBox->setChecked(srvConfig->reverse == 1);
        ui->colorF->setText("Green");
    } else {
        ui->colorF->setEnabled(true);
        ui->minValF->setEnabled(false);
        ui->midValF->setEnabled(false);
        ui->maxValF->setEnabled(false);
        ui->reverseBox->setEnabled(false);

        PortConfig::LEDConfig * ledConfig = (PortConfig::LEDConfig *)*outConfig;
        ui->minValF->setText("-127");
        ui->midValF->setText("0");
        ui->maxValF->setText("128");
        ui->reverseBox->setChecked(false);
        ui->colorF->setText(ledConfig->color);
    }
    QWidget::show();
}

void OutputPortConfigWidget::accepted() {
    delete (*outConfig);
    if(ui->typeCB->currentText() == "SRV") {
        PortConfig::SRVConfig * srvConfig = new PortConfig::SRVConfig;
        srvConfig->type = "SRV";
        srvConfig->portNum = ui->portNumLBL->text().toInt();
        srvConfig->minVal = ui->minValF->text().toInt();
        srvConfig->midVal = ui->midValF->text().toInt();
        srvConfig->maxVal = ui->maxValF->text().toInt();
        srvConfig->reverse = ui->reverseBox->isChecked();
        (*outConfig) = srvConfig;

    } else if(ui->typeCB->currentText() == "DOUT") {
        PortConfig::LEDConfig * ledConfig = new PortConfig::LEDConfig;
        ledConfig->type = "DOUT";
        ledConfig->portNum = ui->portNumLBL->text().toInt();
        ledConfig->color = ui->colorF->text();
        (*outConfig) = ledConfig;
    }
}

void OutputPortConfigWidget::cbChanged(const QString &text) {
    if(text == "DOUT") {
        ui->colorF->setEnabled(true);
        ui->minValF->setEnabled(false);
        ui->midValF->setEnabled(false);
        ui->maxValF->setEnabled(false);
        ui->reverseBox->setEnabled(false);
    } else if(text == "SRV") {
        ui->colorF->setEnabled(false);
        ui->minValF->setEnabled(true);
        ui->midValF->setEnabled(true);
        ui->maxValF->setEnabled(true);
        ui->reverseBox->setEnabled(true);
    }
}
