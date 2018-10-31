#ifndef SHOWEDITORWINDOW_H
#define SHOWEDITORWINDOW_H

#include <QMainWindow>

class ControllerAdapter;
class SerialSettingsDialog;
class RecordingConfigurationDialog;
class QAction;
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

    void connect_triggered();

    void disconnect_triggered();

    void settings_triggered();

    void play_triggered();

    void pause_triggered();

    void stop_triggered();

    void record_triggered();

    void configure_recording_triggered();

private:
    enum PlaybackState {STOPPED, PLAYING, PAUSED, RECORDING};

    enum PlaybackEvent {PLAY, PAUSE, STOP, RECORD};

    //Key References
    Ui::ShowEditorWindow *ui;
    ShowPrimaryPanel *panel;
    ControllerAdapter * adapter;
    SerialSettingsDialog * settingsDialog;
    RecordingConfigurationDialog * recordingConfigurationDialog;

    //Internal Variables
    bool recordingConfigured = false;
    bool serialOpen = false;
    PlaybackState playbackState = STOPPED;

    //Actions
    QAction * connectAct;
    QAction * disconnectAct;
    QAction * settingsAct;
    QAction * playAct;
    QAction * pauseAct;
    QAction * stopAct;
    QAction * recordAct;
    QAction * configureRecordingAct;

    //Key Functions
    void createSerialToolbar();
    void createPreviewingToolbar();

    //Miscilaneous Functions
    void updatePlaybackState(PlaybackEvent playbackEvent);
    void setEnabledActions();
    void serialDisconnected(); //To be called if serial is disconnected for any reason
};

#endif // SHOWEDITORWINDOW_H
