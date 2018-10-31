#include "showeditorwindow.h"
#include "ui_showeditorwindow.h"
#include "track.h"
#include "showprimarypanel.h"
#include "showbaseclass.h"
#include <QFileDialog>
#include <QDebug>

#include "controlleradapter.h"
#include "serialsettingsdialog.h"
#include "recordingconfigurationdialog.h"

ShowEditorWindow::ShowEditorWindow(ControllerAdapter * adapter, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ShowEditorWindow)
{
    ui->setupUi(this);
    createSerialToolbar();
    createPreviewingToolbar();
    this->adapter = adapter;
    panel = new ShowPrimaryPanel(this, adapter);
    settingsDialog = new SerialSettingsDialog(this);
    recordingConfigurationDialog = new RecordingConfigurationDialog(this);

    //panel->resize(500, 500);
    //panel->setMinimumSize(1000, 500);
    //panel->setMaximumSize(500,500);
    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea->setWidget(panel);


    //Set Actions Enabled or Disabled
    connectAct->setEnabled(false);
    disconnectAct->setEnabled(false);
    settingsAct->setEnabled(true);
    playAct->setEnabled(false);
    pauseAct->setEnabled(false);
    stopAct->setEnabled(false);
    recordAct->setEnabled(false);
    configureRecordingAct->setEnabled(false);
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
    panel->save();
}

void ShowEditorWindow::on_actionSave_As_triggered()
{
    panel->saveAs();
}

void ShowEditorWindow::connect_triggered()
{
    SerialSettingsDialog::Settings settings = settingsDialog->settings();
    int result = adapter->startSerialConnection(settings.name, QSerialPort::BaudRate(settings.baudRate));
    if(result != 0)
        return; //TODO issue error dialog
    playAct->setEnabled(true);
    serialOpen = true;
    setEnabledActions();
    connectAct->setEnabled(false);
    disconnectAct->setEnabled(true);
}

void ShowEditorWindow::disconnect_triggered()
{
    adapter->stopSerialConnection();
    serialDisconnected();
}

void ShowEditorWindow::settings_triggered()
{
    settingsDialog->show();
    settingsDialog->exec();
    int result = settingsDialog->result();
    if(result != QDialog::Accepted)
        return;
    connectAct->setEnabled(true);
}

void ShowEditorWindow::play_triggered()
{
    if(playbackState == STOPPED)
        panel->transferShow();
    panel->startShow();//TODO transfers need to be queued and stop recording needs to block
    updatePlaybackState(PLAY);
}

void ShowEditorWindow::pause_triggered()
{
    panel->pauseShow();
    updatePlaybackState(PAUSE);
}

void ShowEditorWindow::stop_triggered()
{
    if(playbackState == PLAYING || playbackState == PAUSED)
        panel->stopShow();
    else if(playbackState == RECORDING)
        panel->stopRecording();
    updatePlaybackState(STOP);
}

void ShowEditorWindow::record_triggered()
{
    panel->startRecording();
    updatePlaybackState(RECORD);
}

void ShowEditorWindow::configure_recording_triggered()
{
    recordingConfigurationDialog->show();
    recordingConfigurationDialog->exec();
    int result = recordingConfigurationDialog->result();
    if(result != QDialog::Accepted)
        return;

    panel->transferShow();
    QStringList args;
    RecordingConfigurationDialog::Settings settings = recordingConfigurationDialog->settings;
    if(settings.recordingType == RecordingConfigurationDialog::MOTOR_RECORDING)
        args << "MOT" << settings.motorPort << QString::number(settings.maxVal) << QString::number(settings.minVal) << QString::number(settings.midVal) << QString::number(settings.reverse);
    else
        args << "LED" << settings.ledPort;
    panel->configureRecording(&args);
    recordingConfigured = true;
    setEnabledActions();
}


void ShowEditorWindow::createSerialToolbar()
{
    QMenu * serialMenu = menuBar()->addMenu(tr("&Serial"));
    QToolBar *serialToolBar = addToolBar(tr("Serial"));

    const QIcon connectIcon = QIcon(":/images/Connect.png");
    connectAct = new QAction(connectIcon, tr("Connect"), this);
    //configureRecordingAct->setShortcut();
    connectAct->setStatusTip(tr("Open Serial Port for Data Transfer"));
    connect(connectAct, &QAction::triggered, this, &ShowEditorWindow::connect_triggered);
    serialMenu->addAction(connectAct);
    serialToolBar->addAction(connectAct);

    const QIcon disconnectIcon = QIcon(":/images/Disconnect.png");
    disconnectAct = new QAction(disconnectIcon, tr("Disconnect"), this);
    //configureRecordingAct->setShortcut();
    disconnectAct->setStatusTip(tr("Close Serial Port"));
    connect(disconnectAct, &QAction::triggered, this, &ShowEditorWindow::disconnect_triggered);
    serialMenu->addAction(disconnectAct);
    serialToolBar->addAction(disconnectAct);

    const QIcon settingsIcon = QIcon(":/images/Settings.png");
    settingsAct = new QAction(settingsIcon, tr("Settings"), this);
    //configureRecordingAct->setShortcut();
    settingsAct->setStatusTip(tr("Configure Serial Connection"));
    connect(settingsAct, &QAction::triggered, this, &ShowEditorWindow::settings_triggered);
    serialMenu->addAction(settingsAct);
    serialToolBar->addAction(settingsAct);
}

void ShowEditorWindow::createPreviewingToolbar()
{
    QMenu * previewingMenu = menuBar()->addMenu(tr("&Previewing"));
    QToolBar *previewingToolBar = addToolBar(tr("Previewing"));

    const QIcon playIcon = QIcon(":/images/Play.png");
    playAct = new QAction(playIcon, tr("Play Show"), this);
    //configureRecordingAct->setShortcut();
    playAct->setStatusTip(tr("Play Current Show on Controller"));
    connect(playAct, &QAction::triggered, this, &ShowEditorWindow::play_triggered);
    previewingMenu->addAction(playAct);
    previewingToolBar->addAction(playAct);

    const QIcon pauseIcon = QIcon(":/images/Pause.png");
    pauseAct = new QAction(pauseIcon, tr("Pause Show"), this);
    //configureRecordingAct->setShortcut();
    pauseAct->setStatusTip(tr("Pause Currently Playing Show on Controller"));
    connect(pauseAct, &QAction::triggered, this, &ShowEditorWindow::pause_triggered);
    previewingMenu->addAction(pauseAct);
    previewingToolBar->addAction(pauseAct);

    const QIcon stopIcon = QIcon(":/images/Stop.png");
    stopAct = new QAction(stopIcon, tr("Stop Show"), this);
    //configureRecordingAct->setShortcut();
    stopAct->setStatusTip(tr("Stop Currently Playing Show or Recording on Controller"));
    connect(stopAct, &QAction::triggered, this, &ShowEditorWindow::stop_triggered);
    previewingMenu->addAction(stopAct);
    previewingToolBar->addAction(stopAct);

    const QIcon recordIcon = QIcon(":/images/Record.png");
    recordAct = new QAction(recordIcon, tr("Start Recording"), this);
    //configureRecordingAct->setShortcut();
    recordAct->setStatusTip(tr("Start Recording as Configured in the Configure Step"));
    connect(recordAct, &QAction::triggered, this, &ShowEditorWindow::record_triggered);
    previewingMenu->addAction(recordAct);
    previewingToolBar->addAction(recordAct);

    const QIcon configureIcon = QIcon(":/images/Wrench.png");
    configureRecordingAct = new QAction(configureIcon, tr("Configure &Recording"), this);
    //configureRecordingAct->setShortcut();
    configureRecordingAct->setStatusTip(tr("Configure Recording Details"));
    connect(configureRecordingAct, &QAction::triggered, this, &ShowEditorWindow::configure_recording_triggered);
    previewingMenu->addAction(configureRecordingAct);
    previewingToolBar->addAction(configureRecordingAct);


}


void ShowEditorWindow::updatePlaybackState(PlaybackEvent playbackEvent)
{
    switch(playbackEvent) {
    case PLAY:
        playbackState = PLAYING;
        break;
    case PAUSE:
        playbackState = PAUSED;
        break;
    case STOP:
        playbackState = STOPPED;
        break;
    case RECORD:
        playbackState = RECORDING;
        break;
    }
    setEnabledActions();
}

void ShowEditorWindow::setEnabledActions()
{
    if(serialOpen) {
        switch(playbackState) {
        case STOPPED:
            playAct->setEnabled(true);
            pauseAct->setEnabled(false);
            stopAct->setEnabled(false);
            recordAct->setEnabled(recordingConfigured);
            configureRecordingAct->setEnabled(true);
            break;
        case PLAYING:
            playAct->setEnabled(false);
            pauseAct->setEnabled(true);
            stopAct->setEnabled(true);
            recordAct->setEnabled(false);
            configureRecordingAct->setEnabled(false);
            break;
        case PAUSED:
            playAct->setEnabled(true);
            pauseAct->setEnabled(false);
            stopAct->setEnabled(true);
            recordAct->setEnabled(false);
            configureRecordingAct->setEnabled(false);
            break;
        case RECORDING:
            playAct->setEnabled(false);
            pauseAct->setEnabled(false);
            stopAct->setEnabled(true);
            recordAct->setEnabled(false);
            configureRecordingAct->setEnabled(false);
            break;
        }
    } else { //Serial not open
        playAct->setEnabled(false);
        pauseAct->setEnabled(false);
        stopAct->setEnabled(false);
        recordAct->setEnabled(false);
        configureRecordingAct->setEnabled(false);
    }
}

void ShowEditorWindow::serialDisconnected() {
    serialOpen = false;
    recordingConfigured = false;
    playbackState = STOPPED;
    connectAct->setEnabled(true);
    disconnectAct->setEnabled(false);
    setEnabledActions();
}

