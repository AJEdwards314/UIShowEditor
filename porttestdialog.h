#ifndef PORTTESTDIALOG_H
#define PORTTESTDIALOG_H

#include <QDialog>

namespace Ui {
class PortTestDialog;
}

class PortReadDaemon;

class PortTestDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PortTestDialog(QWidget *parent = nullptr);
    ~PortTestDialog();

public slots:
    void readReturned(int val);

signals:
    void closeRead();

private slots:
    void on_buttonBox_accepted();

    void on_inputCB_stateChanged(int arg1);

    void on_outputCB_stateChanged(int arg1);

    void on_inoutLinkCB_stateChanged(int arg1);

    void on_outputCombo_currentTextChanged(const QString &arg1);

    void on_inputCombo_currentTextChanged(const QString &arg1);

    void on_testButton_clicked();

    void on_buttonBox_rejected();

private:
    Ui::PortTestDialog *ui;

    QString inPort;
    QString outPort;
    int driveVal;
    int readVal;
    bool driving;
    bool reading;
    bool linked;

    PortReadDaemon * daemon;

    void setInPort(QString inPort);
    void setOutPort(QString outPort);
    void setDriveVal(int driveVal);
    void setReadVal(int readVal);
    void setDriving(bool driving);
    void setReading(bool reading);
    void setLinked(bool linked);

    int getLinkedDriveVal();
    void startRead();



};

#endif // PORTTESTDIALOG_H
