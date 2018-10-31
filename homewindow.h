#ifndef HOMEWINDOW_H
#define HOMEWINDOW_H
#include "controlleradapter.h"
#include <QMainWindow>

namespace Ui {
class HomeWindow;
}

class HomeWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HomeWindow(ControllerAdapter *adapter, QWidget *parent = nullptr);
    ~HomeWindow();


private slots:
    void on_behaviorButton_clicked();

    void on_assemblerButton_clicked();

private:
    Ui::HomeWindow *ui;
    ControllerAdapter *adapter;
};

#endif // HOMEWINDOW_H
