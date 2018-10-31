#ifndef RECORDINGCONFIGURATIONDIALOG_H
#define RECORDINGCONFIGURATIONDIALOG_H

#include <QDialog>


namespace Ui {
class RecordingConfiguration;
}

class RecordingConfigurationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RecordingConfigurationDialog(QWidget *parent = nullptr);
    ~RecordingConfigurationDialog();

    enum RecordingType {
        MOTOR_RECORDING,
        LED_RECORDING
    };

    struct Settings {
        RecordingType recordingType;
        QString motorPort;
        int maxVal;
        int minVal;
        int midVal;
        bool reverse;
        QString ledPort;
    };

    Settings settings;

private slots:
    void on_buttonBox_accepted();

    void on_motorRB_clicked(bool checked);

    void on_ledRB_clicked(bool checked);

private:
    Ui::RecordingConfiguration *ui;

    void initializeSettings();
    void updateSettings();
    void setChecked(RecordingType recordingType);
};

#endif // RECORDINGCONFIGURATIONDIALOG_H
