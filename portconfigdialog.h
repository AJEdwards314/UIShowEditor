#ifndef PORTCONFIGDIALOG_H
#define PORTCONFIGDIALOG_H

#include <QDialog>
#include <QList>

#include "portconfig.h"

class OutputPortConfigWidget;
class InputPortConfigWidget;

namespace Ui {
class PortConfigDialog;
}

class PortConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PortConfigDialog(PortConfig * config, QWidget *parent = nullptr);
    ~PortConfigDialog();

private slots:
    void onAccept();

private:
    Ui::PortConfigDialog *ui;
    PortConfig * config;
    QList<OutputPortConfigWidget *> outputWidgets;
    QList<InputPortConfigWidget *> inputWidgets;
};

#endif // PORTCONFIGDIALOG_H
