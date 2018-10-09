#ifndef WAVDIALOG_H
#define WAVDIALOG_H

#include <QDialog>

class QAbstractButton;
class WAVShowBarWidget;

namespace Ui {
class WAVDialog;
}

class WAVDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WAVDialog(QWidget *parent, QString filename,int offset, QString port);
    ~WAVDialog();

protected:
    WAVShowBarWidget *parentShowBar;

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_clicked(QAbstractButton *button);

    void applyToParent();

private:
    Ui::WAVDialog *ui;
};

#endif // WAVDIALOG_H
