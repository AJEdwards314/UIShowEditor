#include "porttestdialog.h"

#include <QDebug>

#include "ui_porttestdialog.h"
#include "controlleradapter.h"
#include "portconfig.h"
#include "portreaddaemon.h"

PortTestDialog::PortTestDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PortTestDialog)
{
    driving = false;
    reading = false;
    linked = false;
    ui->setupUi(this);
    ui->inputCombo->addItems(PortConfig::getInstance()->getPorts("IN"));
    ui->outputCombo->addItems(PortConfig::getInstance()->getPorts("OUT"));
    inPort = ui->inputCombo->currentText();
    setInPort(inPort);
    outPort = ui->outputCombo->currentText();
    setOutPort(outPort);
    daemon = nullptr;

}

PortTestDialog::~PortTestDialog()
{
    delete ui;
}

void PortTestDialog::readReturned(int val)
{
    setReadVal(val);
}

void PortTestDialog::on_buttonBox_accepted()
{
    setDriving(false);
    setReading(false);
    setLinked(false);
    closeRead();
}

void PortTestDialog::on_buttonBox_rejected()
{
    setDriving(false);
    setReading(false);
    setLinked(false);
    closeRead();
}

void PortTestDialog::on_inputCB_stateChanged(int arg1)
{
    setReading(arg1 != 0);
}

void PortTestDialog::on_outputCB_stateChanged(int arg1)
{
    setDriving(arg1 != 0);
}

void PortTestDialog::on_inoutLinkCB_stateChanged(int arg1)
{
    setLinked(arg1 != 0);
}

void PortTestDialog::on_inputCombo_currentTextChanged(const QString &arg1)
{
    setInPort(arg1);
}

void PortTestDialog::on_outputCombo_currentTextChanged(const QString &arg1)
{
    setOutPort(arg1);
}



void PortTestDialog::setInPort(QString inPort)
{
    if(this->inPort == inPort)
        return;
    ui->outputCombo->setCurrentText(inPort);
    this->inPort = inPort;
    if(reading) {
        closeRead();
        startRead();
    }
}

void PortTestDialog::setOutPort(QString outPort)
{
    if(this->outPort == outPort)
        return;
    ui->outputCombo->setCurrentText(outPort);
    if(driving) {
        ControllerAdapter::getInstance()->killPort(this->outPort);
    }
    this->outPort = outPort;
    this->driveVal = 9999;
    PortConfig::SRVConfig * config = (PortConfig::SRVConfig *) PortConfig::getInstance()->getOutputConfig(outPort);
    setDriveVal(config->midVal);
}

void PortTestDialog::setDriveVal(int driveVal)
{
    if(this->driveVal == driveVal)
        return;
    this->driveVal = driveVal;
    ui->outputTB->setText(QString::number(driveVal));
    if(driving) {
        ControllerAdapter::getInstance()->drivePort(outPort, driveVal);
    }
}

void PortTestDialog::setReadVal(int readVal)
{
    if(this->readVal == readVal)
        return;
    this->readVal = readVal;
    ui->inputLbl->setText("Read Value = " + QString::number(readVal));
    if(linked) {
        setDriveVal(getLinkedDriveVal());
    }
}

void PortTestDialog::setDriving(bool driving)
{
    if(this->driving == driving)
        return;
    this->driving = driving;
    ui->outputCB->setChecked(driving);
    if(driving)
        ControllerAdapter::getInstance()->drivePort(outPort, driveVal);
    else
        ControllerAdapter::getInstance()->killPort(outPort);
}

void PortTestDialog::setReading(bool reading)
{
    if(this->reading == reading)
        return;
    this->reading = reading;
    ui->inputCB->setChecked(reading);
    if(reading)
        startRead();
    else
        closeRead();
}

void PortTestDialog::setLinked(bool linked)
{
    if(this->linked == linked)
        return;
    this->linked = linked;
    ui->inoutLinkCB->setChecked(linked);
    ui->outputTB->setReadOnly(linked);
    if(linked) {
        if(reading)
            setDriveVal(getLinkedDriveVal());
    }
}

int PortTestDialog::getLinkedDriveVal()
{
    if(PortConfig::getType(outPort) == "SRV") {
        PortConfig::SRVConfig * config = (PortConfig::SRVConfig *) PortConfig::getInstance()->getOutputConfig(outPort);
        if(PortConfig::getType(inPort) != "AIN")
            return config->midVal;
        double scale = readVal / 512.0 * (1 - 2 * config->reverse);
        qInfo() << "Scale = " << QString::number(scale);
        int newDriveVal;
        if(scale > 0)
            newDriveVal = (config->maxVal - config->midVal) * scale + config->midVal;
        else
            newDriveVal = (config->midVal - config->minVal) * scale + config->midVal;
        return newDriveVal;
    } else {
        PortConfig::LEDConfig * config = (PortConfig::LEDConfig *) PortConfig::getInstance()->getOutputConfig(outPort);
        if(PortConfig::getType(inPort) != "DIN")
            return 0;
        return readVal;
    }
}

void PortTestDialog::startRead()
{
    daemon = new PortReadDaemon(this, inPort);
    daemon->start();
}



void PortTestDialog::on_testButton_clicked()
{
    if(!linked) {
        bool ok;
        int newDriveVal = ui->outputTB->text().toInt(&ok);
        if(!ok)
            return;
        setDriveVal(newDriveVal);
    }
}


