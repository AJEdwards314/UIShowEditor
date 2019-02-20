#ifndef HOMEWINDOW_H
#define HOMEWINDOW_H
#include <QMainWindow>

class ControllerAdapter;

namespace Ui {
class HomeWindow;
}

class HomeWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HomeWindow(QWidget *parent = nullptr);
    ~HomeWindow() override;


private slots:
    void on_behaviorButton_clicked();

    void on_assemblerButton_clicked();

private:
    Ui::HomeWindow *ui;
    void showEvent(QShowEvent * event) override;
};

#endif // HOMEWINDOW_H
