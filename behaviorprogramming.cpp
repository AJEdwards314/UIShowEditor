#include "behaviorprogramming.h"
#include "ui_behaviorprogramming.h"
#include <QDir>


BehaviorProgramming::BehaviorProgramming(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BehaviorProgramming)
{
    ui->setupUi(this);
    ui->textFrame->setHidden(true);
    ui->thresholdEdit_2->setDisabled(true);
    ui->thresholdEdit_3->setDisabled(true);
    setWindowTitle("Unsaved");

    QDir directory("C:/Users/Morga/Documents/AnimaniacsGUI/Shows");
    directory.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    QFileInfoList list = directory.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
            QFileInfo fileInfo = list.at(i);
            ui->showNameMenu->addItem(fileInfo.fileName());
        }
}
BehaviorProgramming::~BehaviorProgramming()
{
    delete ui;
}

void BehaviorProgramming::on_actionNew_triggered()
{
    currentFile.clear();
    ui->triggerNameEdit->clear();
    ui->inTypeMenu->setCurrentIndex(0);
    ui->inPortMenu->setCurrentIndex(0);
    ui->showNameMenu->setCurrentIndex(0);
    ui->logicCombo->setCurrentIndex(0);
    ui->triggerList->clear();
    ui->thresholdEdit->clear();
    ui->thresholdEdit_2->clear();
    ui->thresholdEdit_3->clear();
    ui->commandText->clear();
    ui->analogFrame->setHidden(false);
    ui->textFrame->setHidden(true);
    ui->thresholdEdit->setEnabled(true);
    ui->thresholdEdit_2->setDisabled(true);
    ui->thresholdEdit_3->setDisabled(true);

    setWindowTitle("Unsaved");
}


void BehaviorProgramming::on_actionSave_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save as") + ".bmo";
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, "Warning", "Cannot save file : " + file.errorString());
        return;
    }
    QFileInfo fileInfo(file.fileName());
    fileName = fileInfo.fileName();
    currentFile = fileName;
    setWindowTitle(fileName);
    QTextStream out(&file);
    out << "Title," + fileName +"\n";
    out << "Triggers\n";
    for(int i = 0; i < ui->triggerList->count(); i++)
    {
            QListWidgetItem *item = ui->triggerList->item(i);
            out << item->text() +"\n";
    }
    out << "End Triggers";

}

void BehaviorProgramming::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open the file");
    QFile file(fileName);
    QFileInfo fileInfo(file.fileName());
    fileName = fileInfo.fileName();
    currentFile = fileName;
    if(!file.open(QIODevice::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, "Warning", "Cannot open file : " + file.errorString());
        return;
    }
    setWindowTitle(fileName);
    ui->analogFrame->setHidden(false);
    ui->textFrame->setHidden(true);
    ui->thresholdEdit->setEnabled(true);
    ui->thresholdEdit_2->setDisabled(true);
    ui->thresholdEdit_3->setDisabled(true);

    QTextStream in(&file);
    QString text;
    text = in.readLine();
    text = in.readLine();
    if(text != "Triggers")
    {
        QMessageBox::warning(this, "Warning", "Not a .bmo file! " );
        return;
    }
    text = in.readLine();
    while(text!="End Triggers")
    {
        ui->triggerList->addItem(text);
        text = in.readLine();
    }
    file.close();

}

void BehaviorProgramming::on_inTypeMenu_activated(const QString &arg1)
{
    if(arg1 == "Analog" || arg1 == "Digital")
    {
        ui->textFrame->setHidden(true);
        ui->textFrame->setDisabled(true);
        ui->analogFrame->setEnabled(true);
        ui->analogFrame->setHidden(false);
    }
    else
    {
        ui->analogFrame->setHidden(true);
        ui->analogFrame->setDisabled(true);
        ui->textFrame->setEnabled(true);
        ui->textFrame->setHidden(false);
    }
}

void BehaviorProgramming::on_logicCombo_activated(const QString &arg1)
{
    if(arg1 == "< X <")
    {
        ui->thresholdEdit->setDisabled(true);
        ui->thresholdEdit_2->setEnabled(true);
        ui->thresholdEdit_3->setEnabled(true);
    }
    else
    {
        ui->thresholdEdit->setEnabled(true);
        ui->thresholdEdit_2->setDisabled(true);
        ui->thresholdEdit_3->setDisabled(true);
    }
}

void BehaviorProgramming::on_addTrigger_clicked()
{
    QString trigger;
    if(ui->inTypeMenu->currentText() == "Text")
    {
        trigger = ui->triggerNameEdit->text() +"," + ui->inTypeMenu->currentText() +
                "," + ui->inPortMenu->currentText() +"," + ui->commandText->text() + "," + ui->showNameMenu->currentText();
    }
    else
    {
        if(ui->logicCombo->currentText() == "< X <")
        {
            trigger = ui->triggerNameEdit->text() +"," + ui->inTypeMenu->currentText() +
                    "," + ui->inPortMenu->currentText() +"," + ui->logicCombo->currentText()
                    + "," + ui->thresholdEdit_2->text() + "," + ui->thresholdEdit_3->text() +"," + ui->showNameMenu->currentText();
        }
        else
        {
            trigger = ui->triggerNameEdit->text() +"," + ui->inTypeMenu->currentText() +
                    "," + ui->inPortMenu->currentText() +"," + ui->logicCombo->currentText()
                    + "," +  ui->thresholdEdit->text() +"," + ui->showNameMenu->currentText();
        }
    }
    ui->triggerList->addItem(trigger);
}
