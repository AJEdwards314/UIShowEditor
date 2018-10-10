#ifndef SHOWPRIMARYPANEL_H
#define SHOWPRIMARYPANEL_H

#include<QWidget>
class Track;
class ShowBaseClass;
class ShowEditorWindow;
class QFile;

class ShowPrimaryPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ShowPrimaryPanel(QWidget *parent = nullptr); //Parent will be of type ShowEditorWindow

    //Show and Track Handling
    void openShow(QString filename);
    void createEmptyShow();
    void newShow();
    void openTracks(QStringList &filenames, QList<int> *offsets = nullptr, QList<QString> *ports = nullptr, QList<QList<QString>> *args = nullptr);
    void trackShowDataUpdated();
    void removeTrack(Track* track);
    void save();
    void saveAs(QFile *newSourceFile);
    inline bool hasShow() {return showBase != nullptr;}
    inline QList<Track*>* getTracks() {return tracks;}

    //Misc Functions
    void updateTitle();
    void objectGrabbed(Track* widget);
protected:
    //Key References
    ShowEditorWindow *parentWindow;

    //Key Children
    ShowBaseClass *showBase;
    QList<Track*> * tracks;

    //Attributes
    Track *grabbedTrack = nullptr;
    int originalSlot = -1, currentSlot = -1;
    float pixpersec = 100;
    bool showChanged = false;

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

signals:

public slots:
};

#endif // SHOWPRIMARYPANEL_H
