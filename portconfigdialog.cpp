#include "portconfigdialog.h"
#include "ui_portconfigdialog.h"

#include "portconsts.h"
#include "outputportconfigwidget.h"
#include "inputportconfigwidget.h"

PortConfigDialog::PortConfigDialog(PortConfig * config, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PortConfigDialog)
{
    ui->setupUi(this);
    this->config = config;

    connect(this, &PortConfigDialog::accepted, this, &PortConfigDialog::onAccept);

    QFrame * outputFrame = new QFrame;
    QLayout * ol = new QVBoxLayout(outputFrame);
    ui->outputPortsArea->setWidget(outputFrame);
    ui->outputPortsArea->setWidgetResizable(true);
    for(int i = 0; i < PortConsts::numOutPorts; i++) {
        OutputPortConfigWidget * widget = new OutputPortConfigWidget(&config->outConfigs[i]);
        outputWidgets.append(widget);
        ol->addWidget(widget);
    }

    QFrame * inputFrame = new QFrame;
    QLayout * il = new QVBoxLayout(inputFrame);
    ui->inputPortsArea->setWidget(inputFrame);
    ui->inputPortsArea->setWidgetResizable(true);
    for(int i = 0; i < PortConsts::numInPorts; i++) {
        InputPortConfigWidget * widget = new InputPortConfigWidget(&config->inConfigs[i]);
        inputWidgets.append(widget);
        il->addWidget(widget);
    }
}

PortConfigDialog::~PortConfigDialog()
{
    delete ui;
}

void PortConfigDialog::onAccept() {
    for(int i = 0; i < outputWidgets.length(); i++) {
        outputWidgets[i]->accepted();
    }
    for(int i = 0; i < inputWidgets.length(); i++) {
        inputWidgets[i]->accepted();
    }
    config->save();
}
