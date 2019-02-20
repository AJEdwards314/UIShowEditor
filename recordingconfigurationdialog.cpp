#include "recordingconfigurationdialog.h"
#include "ui_recordingconfiguration.h"
#include "portconfig.h"

RecordingConfigurationDialog::RecordingConfigurationDialog(QWidget * parent) :
    QDialog (parent),
    ui(new Ui::RecordingConfiguration)
{
    ui->setupUi(this);
    initializeSettings();
}

RecordingConfigurationDialog::~RecordingConfigurationDialog()
{
    delete ui;
}

void RecordingConfigurationDialog::on_buttonBox_accepted()
{
    updateSettings();
}

void RecordingConfigurationDialog::on_motorRB_clicked(bool checked)
{
    RecordingType recordingType = checked ? MOTOR_RECORDING : LED_RECORDING;
    setChecked(recordingType);
}

void RecordingConfigurationDialog::on_ledRB_clicked(bool checked)
{
    RecordingType recordingType = checked ? LED_RECORDING : MOTOR_RECORDING;
    setChecked(recordingType);
}


void RecordingConfigurationDialog::initializeSettings()
{
    ui->motorPortCB->addItems(PortConfig::getInstance()->getPorts("SRV"));
    ui->ledPortCB->addItems(PortConfig::getInstance()->getPorts("DOUT"));

    ui->motorRB->setChecked(true);
    setChecked(MOTOR_RECORDING);
    updateSettings();
}

void RecordingConfigurationDialog::updateSettings()
{
    if(ui->motorRB->isChecked())
        settings.recordingType = MOTOR_RECORDING;
    else
        settings.recordingType = LED_RECORDING;

    settings.motorPort = ui->motorPortCB->currentText();

    settings.ledPort = ui->ledPortCB->currentText();
}

void RecordingConfigurationDialog::setChecked(RecordingType recordingType)
{
    bool argument = recordingType == MOTOR_RECORDING;
        ui->motorRB->setChecked(argument);
        ui->ledRB->setChecked(!argument);
        ui->motorPortCB->setEnabled(argument);
        ui->ledPortCB->setEnabled(!argument);
}
