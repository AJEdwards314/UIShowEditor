#ifndef LEDDIALOG_H
#define LEDDIALOG_H

#include <QDialog>

class QAbstractButton;
class LEDTrack;

namespace Ui {
class LEDDialog;
}

class LEDDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LEDDialog(QWidget *parent, QString filename, QString name, int offset, QString port, QString colorName);
    ~LEDDialog();

protected:
    LEDTrack *parentTrack; //Reference to Parent - API through apply function

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_clicked(QAbstractButton *button);

    void applyToParent();

private:
    Ui::LEDDialog *ui;
};

#endif // LEDDIALOG_H
