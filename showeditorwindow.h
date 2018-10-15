#ifndef SHOWEDITORWINDOW_H
#define SHOWEDITORWINDOW_H

#include <QMainWindow>

class ControllerAdapter;
namespace Ui {
class ShowEditorWindow;
}
class ShowPrimaryPanel;

class ShowEditorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ShowEditorWindow(ControllerAdapter * adapter, QWidget *parent = nullptr);
    ~ShowEditorWindow() override;

private slots:
    void on_actionNew_triggered();

    void on_actionOpen_Track_triggered();

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionSave_As_triggered();

private:
    //Key References
    Ui::ShowEditorWindow *ui;
    ShowPrimaryPanel *panel;
    ControllerAdapter * adapter;
};

#endif // SHOWEDITORWINDOW_H
