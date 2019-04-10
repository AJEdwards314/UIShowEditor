#include "homewindow.h"
#include "ui_homewindow.h"
#include "behaviorprogramming.h"
#include "showeditorwindow.h"
#include "controlleradapter.h"
#include "portconfigdialog.h"
#include "portconfig.h"
#include "workingdirectory.h"
#include <QDebug>


HomeWindow::HomeWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HomeWindow)
{
    ui->setupUi(this);
//    const QIcon logo = QIcon(":/images/Animaniacs.png");
//    ui->label->pixmap();
}

HomeWindow::~HomeWindow()
{
    delete ui;
}

void HomeWindow::on_behaviorButton_clicked()
{
    BehaviorProgramming *newWindow = new BehaviorProgramming();
    newWindow->show();
}

void HomeWindow::on_assemblerButton_clicked()
{
    ShowEditorWindow *newWindow = new ShowEditorWindow();
    newWindow->show();
}

void HomeWindow::showEvent(QShowEvent * event) {
    QMessageBox message;
    message.setText("Please Select a Working Directory");
    message.exec();
    QString dirString = QFileDialog::getExistingDirectory();
    qInfo() << dirString + "/PortConfig.pcg";
    WorkingDirectory::setDir(dirString);
    PortConfig::setInstance(dirString + "/PortConfig.pcg");
}
