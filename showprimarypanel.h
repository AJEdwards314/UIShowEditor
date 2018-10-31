#ifndef SHOWPRIMARYPANEL_H
#define SHOWPRIMARYPANEL_H

#include<QWidget>
class Track;
class ShowBaseClass;
class ShowEditorWindow;
class QFile;
class ControllerAdapter;

struct Point;

class ShowPrimaryPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ShowPrimaryPanel(QWidget *parent = nullptr, ControllerAdapter * adapter = nullptr); //Parent will be of type ShowEditorWindow

    //Show and Track Handling
    void openShow(QString filename);
    void createEmptyShow();
    void newShow();
    void openTracks(QStringList &filenames, QList<int> *offsets = nullptr, QList<QString> *ports = nullptr, QList<QList<QString>> *args = nullptr);
    void newTrack(QStringList * recordingArgs, QList<Point> * points);
    void trackShowDataUpdated();
    void removeTrack(Track* track);
    void save();
    void saveAs();
    inline bool hasShow() {return showBase != nullptr;}
    inline QList<Track*>* getTracks() {return tracks;}

    //Misc Functions
    void updateTitle();
    void objectGrabbed(Track* widget);

    //UI Handles from Main Window
    void transferShow();
    void startShow();
    void pauseShow();
    void stopShow();
    void configureRecording(QStringList* args);
    void startRecording();
    void stopRecording();

private:
    //Key References
    ShowEditorWindow *parentWindow;
    ControllerAdapter * adapter;

    //Key Children
    ShowBaseClass *showBase;
    QList<Track*> * tracks;

    //Attributes
    Track *grabbedTrack = nullptr;
    int originalSlot = -1, currentSlot = -1;
    float pixpersec = 100;
    bool showChanged = false;
    int showSavedRv, showTransferedRv; //Random values that signifies whether or not a save has occured since the previous show transfer.

    QStringList * recordingArgs;

    //Paint Functions
    void paintEvent(QPaintEvent *) override;

    //Input Handling Functions
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    int calculateSlot(QPoint p);
    void placeAtSlot(Track* track, int index);
    void wheelEvent(QWheelEvent *event) override;

    //Misc Functions
    void updateChildren();
    void generateShowSavedRv();

signals:

public slots:
    void recordingComplete(QList<Point> * points);
};

#endif // SHOWPRIMARYPANEL_H
