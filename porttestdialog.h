#ifndef PORTTESTDIALOG_H
#define PORTTESTDIALOG_H

#include <QDialog>

namespace Ui {
class PortTestDialog;
}

class PortTestDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PortTestDialog(QWidget *parent = nullptr);
    ~PortTestDialog();

private:
    Ui::PortTestDialog *ui;
};

#endif // PORTTESTDIALOG_H
