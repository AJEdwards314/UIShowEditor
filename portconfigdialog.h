#ifndef PORTCONFIGDIALOG_H
#define PORTCONFIGDIALOG_H

#include <QDialog>

namespace Ui {
class PortConfigDialog;
}

class PortConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PortConfigDialog(QWidget *parent = nullptr);
    ~PortConfigDialog();

private:
    Ui::PortConfigDialog *ui;
};

#endif // PORTCONFIGDIALOG_H
