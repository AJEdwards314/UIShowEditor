#include "behaviorprogramming.h"
#include "ui_behaviorprogramming.h"
#include <QDir>
#include <QToolBar>
#include "controlleradapter.h"
#include "serialsettingsdialog.h"
#include "workingdirectory.h"
#include "portconfigdialog.h"
BehaviorProgramming::BehaviorProgramming(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BehaviorProgramming)
{
    ui->setupUi(this);
    ui->textFrame->setHidden(true);
    ui->digitalFrame->setHidden(true);
    ui->thresholdEdit_2->setDisabled(true);
    ui->thresholdEdit_3->setDisabled(true);
    settingsDialog = new SerialSettingsDialog(this);
    setWindowTitle("Unsaved");

    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setEnabled(false);
    ui->actionSettings->setEnabled(true);

    createSerialToolbar();
    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setEnabled(false);

    createToolsToolbar();
    portConfigAct->setEnabled(true);
    portTestAct->setEnabled(false);
    portConfigDialog = new PortConfigDialog(PortConfig::getInstance());



    QDir directory = WorkingDirectory::getDir();
    directory.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    QFileInfoList list = directory.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        if(fileInfo.suffix() == "shw")
            ui->showNameMenu->addItem(fileInfo.fileName());
    }

    inputType = "AIN";
    updatePortList();
    //QDir directory(":/Shows");

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
    ui->digitalFrame->setHidden(true);
    ui->thresholdEdit->setEnabled(true);
    ui->thresholdEdit_2->setDisabled(true);
    ui->thresholdEdit_3->setDisabled(true);

    setWindowTitle("Unsaved");
}


void BehaviorProgramming::on_actionSave_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save as", WorkingDirectory::getPath(), tr("Animaniacs Behavior Model (*.bmo)"));
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

    file.close();

    /*QDir directory = fileInfo.absoluteDir();
    directory.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    QFileInfoList list = directory.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        if(fileInfo.suffix() == "shw")
            ui->showNameMenu->addItem(fileInfo.fileName());
    }*/


}

void BehaviorProgramming::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open the file", WorkingDirectory::getPath(), tr("Animaniacs Behavior Model (*.bmo)"));
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
    ui->analogFrame->setHidden(true);
    ui->textFrame->setHidden(true);
    ui->digitalFrame->setHidden(false);
    ui->thresholdEdit->setEnabled(true);
    ui->thresholdEdit_2->setDisabled(true);
    ui->thresholdEdit_3->setDisabled(true);

    QTextStream in(&file);
    QString text;
    text = in.readLine();
    text = in.readLine();
    if(text != "Triggers")
    {
        QMessageBox::warning(this, "Warning", "File Corrupted" );
        return;
    }
    text = in.readLine();
    while(text!="End Triggers")
    {
        ui->triggerList->addItem(text);
        text = in.readLine();
    }
    file.close();

    /*QDir directory = fileInfo.absoluteDir();
    directory.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    QFileInfoList list = directory.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        if(fileInfo.suffix() == "shw")
            ui->showNameMenu->addItem(fileInfo.fileName());
    }*/

}

void BehaviorProgramming::on_inTypeMenu_activated(const QString &arg1)
{
    if(arg1 == "Analog")
    {
        ui->textFrame->setHidden(true);
        ui->textFrame->setDisabled(true);
        ui->analogFrame->setEnabled(true);
        ui->analogFrame->setHidden(false);
        ui->digitalFrame->setHidden(true);
        ui->digitalFrame->setDisabled(true);
        inputType = "AIN";
    }
    else if(arg1 == "Digital")
    {
        ui->textFrame->setHidden(true);
        ui->textFrame->setDisabled(true);
        ui->digitalFrame->setEnabled(true);
        ui->digitalFrame->setHidden(false);
        ui->analogFrame->setHidden(true);
        ui->analogFrame->setDisabled(true);
        inputType = "DIN";
    }
    else
    {
        ui->analogFrame->setHidden(true);
        ui->analogFrame->setDisabled(true);
        ui->textFrame->setEnabled(true);
        ui->textFrame->setHidden(false);
        ui->digitalFrame->setHidden(true);
        ui->digitalFrame->setDisabled(true);
        inputType = "MIC";

    }
    updatePortList();
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
                "," + ui->inPortMenu->currentText() +","+ ui->showNameMenu->currentText() +"," + ui->commandText->text() ;
    }
    else if(ui->inTypeMenu->currentText() == "Digital")
    {
        trigger = ui->triggerNameEdit->text() +"," + ui->inTypeMenu->currentText() +
               "," + ui->inPortMenu->currentText()+"," + ui->showNameMenu->currentText() + "," + ui->digitalValue->currentText();
    }
    else
    {
        if(ui->logicCombo->currentText() == "< X <")
        {
            trigger = ui->triggerNameEdit->text() +"," + ui->inTypeMenu->currentText() +
                    "," + ui->inPortMenu->currentText() +"," +  ui->showNameMenu->currentText()
                    + "," + ui->logicCombo->currentText() +"," + ui->thresholdEdit_2->text() + "," + ui->thresholdEdit_3->text();
        }
        else
        {
            trigger = ui->triggerNameEdit->text() +"," + ui->inTypeMenu->currentText() +
                    "," + ui->inPortMenu->currentText() +"," + ui->showNameMenu->currentText() + "," + ui->logicCombo->currentText()
                    + ","   +  ui->thresholdEdit->text();
        }
    }
    ui->triggerList->addItem(trigger);
}

void BehaviorProgramming::on_actionConnect_triggered()
{
    SerialSettingsDialog::Settings settings = settingsDialog->settings();
    int result = ControllerAdapter::getInstance()->startSerialConnection(settings.name, QSerialPort::BaudRate(settings.baudRate));
    if(result != 0)
        return; //TODO issue error dialog
    serialOpen = true;
    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setEnabled(true);
    portTestAct->setEnabled(true);
}


void BehaviorProgramming::on_actionDisconnect_triggered()
{
    ControllerAdapter::getInstance()->stopSerialConnection();
    serialDisconnected();
}

void BehaviorProgramming::on_actionSettings_triggered()
{
    settingsDialog->show();
    settingsDialog->exec();
    int result = settingsDialog->result();
    if(result != QDialog::Accepted)
        return;
    ui->actionConnect->setEnabled(true);
}

void BehaviorProgramming::port_config_triggered()
{
    portConfigDialog->show();
    portConfigDialog->exec();
    updatePortList();
}

void BehaviorProgramming::port_test_triggered()
{
    ControllerAdapter::getInstance()->sendPortConfig(PortConfig::getInstance()->getSourceFile());
    PortTestDialog * dialog = new PortTestDialog();
    dialog->exec();
}

void BehaviorProgramming::serialDisconnected() {
    serialOpen = false;
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    portConfigAct->setEnabled(true);
    portTestAct->setEnabled(false);
}

void BehaviorProgramming::createSerialToolbar()
{
    QToolBar *serialToolBar = addToolBar(tr("Serial"));

    const QIcon connectIcon = QIcon(":/images/Connect.png");
    ui->actionConnect = new QAction(connectIcon, tr("Connect"), this);
    ui->actionConnect->setStatusTip(tr("Open Serial Port for Data Transfer"));
    connect(ui->actionConnect, &QAction::triggered, this, &BehaviorProgramming::on_actionConnect_triggered);
    serialToolBar->addAction(ui->actionConnect);

    const QIcon disconnectIcon = QIcon(":/images/Disconnect.png");
    ui->actionDisconnect = new QAction(disconnectIcon, tr("Disconnect"), this);
    ui->actionDisconnect->setStatusTip(tr("Close Serial Port"));
    connect(ui->actionDisconnect, &QAction::triggered, this, &BehaviorProgramming::on_actionDisconnect_triggered);
    serialToolBar->addAction(ui->actionDisconnect);

    const QIcon settingsIcon = QIcon(":/images/Settings.png");
    ui->actionSettings= new QAction(settingsIcon, tr("Settings"), this);
    ui->actionSettings->setStatusTip(tr("Configure Serial Connection"));
    connect(ui->actionSettings, &QAction::triggered, this, &BehaviorProgramming::on_actionSettings_triggered);
    serialToolBar->addAction(ui->actionSettings);
}

void BehaviorProgramming::createToolsToolbar() {
    QMenu * toolsMenu = menuBar()->addMenu(tr("&Tools"));
    QToolBar * toolsToolbar = addToolBar(tr("Tools"));

    const QIcon portConfigIcon = QIcon(":/images/PortConfig.png");
    portConfigAct = new QAction(portConfigIcon, tr("Configure Ports"), this);
    portConfigAct->setStatusTip(tr("Set Up Port Output/Input Types and Characteristics"));
    connect(portConfigAct, &QAction::triggered, this, &BehaviorProgramming::port_config_triggered);
    toolsMenu->addAction(portConfigAct);
    toolsToolbar->addAction(portConfigAct);

    const QIcon portTestIcon = QIcon(":/images/PortTest.png");
    portTestAct = new QAction(portTestIcon, tr("Test Port Configuration"));
    portTestAct->setStatusTip(tr("Test Input and Output Ports"));
    connect(portTestAct, &QAction::triggered, this, &BehaviorProgramming::port_test_triggered);
    toolsMenu->addAction(portTestAct);
    toolsToolbar->addAction(portTestAct);
}


void BehaviorProgramming::on_actionUpload_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open the file", WorkingDirectory::getPath(), tr("Animaniacs Behavior Model (*.bmo)"));
    QFile* file = new QFile(fileName);
    ControllerAdapter::getInstance()->transferBehavior(file);
}

void BehaviorProgramming::updatePortList() {
    ui->inPortMenu->clear();
    ui->inPortMenu->addItems(PortConfig::getInstance()->getPorts(inputType));
}


void BehaviorProgramming::on_removeTriggerButton_clicked()
{
    QListWidgetItem *item = ui->triggerList->currentItem();
    if(item != nullptr)
    {
         ui->triggerList->takeItem(ui->triggerList->row(item));
    }

}
