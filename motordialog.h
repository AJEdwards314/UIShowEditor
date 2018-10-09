#ifndef MOTORDIALOG_H
#define MOTORDIALOG_H

#include <QDialog>

class QAbstractButton;
class MotorShowBarWidget;

namespace Ui {
class MotorDialog;
}

class MotorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MotorDialog(QWidget *parent, QString filename, QString name, int offset, QString port, int maxVal, int minVal, int defVal, bool reverse);
    ~MotorDialog();

protected:
    MotorShowBarWidget *parentShowBar;

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_clicked(QAbstractButton *button);

    void applyToParent();

private:
    Ui::MotorDialog *ui;
};

#endif // MotorDIALOG_H
