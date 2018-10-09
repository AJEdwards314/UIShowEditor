#include "showeditorwindow.h"
#include "ui_showeditorwindow.h"
#include "showbarwidget.h"
#include "showprimarypanel.h"
#include "showbaseclass.h"
#include <QFileDialog>
#include <QDebug>

ShowEditorWindow::ShowEditorWindow(QWidget *parent) :
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
