#ifndef SHOWPRIMARYPANEL_H
#define SHOWPRIMARYPANEL_H

#include<QWidget>
class ShowBarWidget;
class ShowBaseClass;
class ShowEditorWindow;
class QFile;

class ShowPrimaryPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ShowPrimaryPanel(QWidget *parent = nullptr); //Parent will be of type ShowEditorWindow
    void objectGrabbed(ShowBarWidget* widget);
    void openShow(QString filename);
    void createEmptyShow();
    void openTracks(QStringList &filenames, QList<int> *offsets = nullptr, QList<QString> *ports = nullptr, QList<QList<QString>> *args = nullptr);
    //inline ShowBaseClass* getShow() {return showBase;}
    inline bool hasShow() {return showBase != nullptr;}
    void save();
    void saveAs(QFile *newSourceFile);
    inline QList<ShowBarWidget*>* getShowBars() {return showBars;}
    void updateTitle();
    void newShow();
    void trackShowDataUpdated();
    void removeTrack(ShowBarWidget* track);
protected:
    //QVBoxLayout *layout;
    void paintEvent(QPaintEvent *) override;
    int originalSlot = -1, currentSlot = -1;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    QList<ShowBarWidget*> * showBars;
    ShowBarWidget *grabbedShowBar = nullptr;
    int calculateSlot(QPoint p);
    void placeAtSlot(ShowBarWidget* showBar, int index);
    float pixpersec = 100;
    void wheelEvent(QWheelEvent *event) override;
    void updateChildren();
    ShowBaseClass *showBase;
    ShowEditorWindow *parentWindow;
    bool showChanged = false;

signals:

public slots:
};

#endif // SHOWPRIMARYPANEL_H
