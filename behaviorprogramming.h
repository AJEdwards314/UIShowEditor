#ifndef BEHAVIORPROGRAMMING_H
#define BEHAVIORPROGRAMMING_H

#include <QMainWindow>

#include<QFile>
#include<QFileDialog>
#include<QTextStream>
#include<QMessageBox>
#include <controlleradapter.h>
#include <serialsettingsdialog.h>
namespace Ui{
class BehaviorProgramming;
}
class BehaviorProgramming : public QMainWindow
{
    Q_OBJECT
public:
    explicit BehaviorProgramming(QWidget *parent = nullptr);
    ~BehaviorProgramming();
private slots:
    void on_actionNew_triggered();

    void on_actionSave_triggered();

    void on_actionOpen_triggered();

    void on_inTypeMenu_activated(const QString &arg1);

    void on_logicCombo_activated(const QString &arg1);

    void on_addTrigger_clicked();

    void on_actionConnect_triggered();


    void on_actionDisconnect_triggered();

    void on_actionSettings_triggered();

    void on_actionUpload_triggered();

private:
    Ui::BehaviorProgramming *ui;
    QString currentFile = "";
    SerialSettingsDialog * settingsDialog;
    bool serialOpen = false;

    void createSerialToolbar();
    void serialDisconnected(); //To be called if serial is disconnected for any reason
};

    #endif // BEHAVIORPROGRAMMIN_H


