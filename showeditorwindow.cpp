#include "showeditorwindow.h"
#include "ui_showeditorwindow.h"
#include "track.h"
#include "showprimarypanel.h"
#include "showbaseclass.h"
#include <QFileDialog>
#include <QDebug>

#include "controlleradapter.h"

ShowEditorWindow::ShowEditorWindow(ControllerAdapter * adapter, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ShowEditorWindow)
{
    ui->setupUi(this);
    panel = new ShowPrimaryPanel(this);

    //panel->resize(500, 500);
    //panel->setMinimumSize(1000, 500);
    //panel->setMaximumSize(500,500);
    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea->setWidget(panel);
    this->adapter = adapter;
}

ShowEditorWindow::~ShowEditorWindow()
{
    delete ui;
}

void ShowEditorWindow::on_actionNew_triggered()
{
    panel->newShow();
}

void ShowEditorWindow::on_actionOpen_Track_triggered()
{
    QStringList filenames = QFileDialog::getOpenFileNames(this, tr("Open Track File"),"",tr("Animaniacs Track Files (*.wav; *.osr; *.lsr)"));
    if(filenames.length() == 0)
        return;
    panel->openTracks(filenames);

    QFile file(filenames[0]);
    qInfo() << "Track Send Returned: " << adapter->sendTrack(&file);
    //qInfo() << "Pause returned: " << adapter.pauseShow();
    return;
}

void ShowEditorWindow::on_actionOpen_triggered()
{
    QString filepath = QFileDialog::getOpenFileName(this, tr("Open Show"),"",tr("Animaniacs Show Files (*.shw)"));
    if(filepath == "")
        return;
    panel->openShow(filepath);
    return;

}

void ShowEditorWindow::on_actionSave_triggered()
{
    if(!panel->hasShow()) {
        panel->createEmptyShow();
        on_actionSave_As_triggered();
        return;
    }
    panel->save();
}

void ShowEditorWindow::on_actionSave_As_triggered()
{
    QString filepath = QFileDialog::getSaveFileName(this, tr("Save Show"),"",tr("Animaniacs Show Files (*.shw)"));
    if(filepath == "")
        return;
    QFile * newSourceFile = new QFile(filepath);
    panel->saveAs(newSourceFile);
}
