#ifndef BEHAVIORPROGRAMMING_H
#define BEHAVIORPROGRAMMING_H

#include <QMainWindow>

#include<QFile>
#include<QFileDialog>
#include<QTextStream>
#include<QMessageBox>


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

private:
    Ui::BehaviorProgramming *ui;
    QString currentFile = "";
};

    #endif // BEHAVIORPROGRAMMIN_H


