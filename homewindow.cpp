#include "homewindow.h"
#include "ui_homewindow.h"
#include "behaviorprogramming.h"
#include "showeditorwindow.h"


HomeWindow::HomeWindow(ControllerAdapter *adapter, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HomeWindow)
{
    ui->setupUi(this);
    this->adapter = adapter;
}

HomeWindow::~HomeWindow()
{
    delete ui;
}

void HomeWindow::on_behaviorButton_clicked()
{;
    BehaviorProgramming *newWindow = new BehaviorProgramming();
    newWindow->show();
}

void HomeWindow::on_assemblerButton_clicked()
{
    ShowEditorWindow *newWindow = new ShowEditorWindow(adapter);
    newWindow->show();
}
