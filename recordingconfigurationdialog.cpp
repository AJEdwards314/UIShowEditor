#include "recordingconfigurationdialog.h"
#include "ui_recordingconfiguration.h"

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
    ui->motorRB->setChecked(true);

    ui->motorPortField->setText("SRV1");

    ui->maxValField->setText("127");

    ui->minValField->setText("-128");

    ui->midValField->setText("0");

    ui->reverseCB->setChecked(false);

    ui->ledPortField->setText("DIO1");
    setChecked(MOTOR_RECORDING);
    updateSettings();
}

void RecordingConfigurationDialog::updateSettings()
{
    if(ui->motorRB->isChecked())
        settings.recordingType = MOTOR_RECORDING;
    else
        settings.recordingType = LED_RECORDING;

    settings.motorPort = ui->motorPortField->text();

    bool ok;
    int parsedInt = ui->maxValField->text().toInt(&ok);
    if(ok)
        settings.maxVal = parsedInt;

    parsedInt = ui->minValField->text().toInt(&ok);
    if(ok)
        settings.minVal = parsedInt;

    parsedInt = ui->midValField->text().toInt(&ok);
    if(ok)
        settings.midVal = parsedInt;

    settings.reverse = ui->reverseCB->isChecked();

    settings.ledPort = ui->ledPortField->text();
}

void RecordingConfigurationDialog::setChecked(RecordingType recordingType)
{
    bool argument = recordingType == MOTOR_RECORDING;
        ui->motorRB->setChecked(argument);
        ui->ledRB->setChecked(!argument);
        ui->motorPortField->setEnabled(argument);
        ui->maxValField->setEnabled(argument);
        ui->minValField->setEnabled(argument);
        ui->midValField->setEnabled(argument);
        ui->reverseCB->setEnabled(argument);
        ui->ledPortField->setEnabled(!argument);
}
