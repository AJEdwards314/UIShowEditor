#define TRACK_HANDLE_WIDTH 10
#define TRACK_INFO_WIDTH 150
#define TRACK_HEIGHT 55
#ifndef TRACK_H
#define TRACK_H



#include <QWidget>

class QFile;
class ShowPrimaryPanel;

class Track : public QWidget
{
    Q_OBJECT
public:
    explicit Track(QWidget *parent = nullptr, float pixpersec = 100, int offset = 0, QFile *sourceFile = nullptr, QString port = "");

    //For saving
    void saveTrackParent();
    void saveTrackAsParent();

    //Setters and Getters
    void setScale(float pixpersec, int width);
    int getBaseWidth();
    inline bool getTrackChanged() {return trackChanged;}
    inline QString getFilename() {return filename;}
    inline QFile * getFile() {return sourceFile;}
    inline int getOffset() {return offset;}
    inline QString getPort() {return port;}
protected:

    //Key References
    ShowPrimaryPanel * parentPanel;
    QFile *sourceFile;

    //Source Attributes (From File)
    int offset;
    QString port;
    QString title;
    QString filename;

    //Generated Intermediate Attributes
    int index;
    int length;
    int idealWidth;
    float augmentedLength;
    float augmentedOffset;
    QRectF handle, infoBox;
    float pixpersec;
    bool trackChanged = false;

    //Paint Functions
    void paintEvent(QPaintEvent *) override;
    void drawHandle(QPainter &painter);

    //Input Handling Functions
    void mousePressEvent(QMouseEvent *event) override;
    void handleClicked(QMouseEvent *event);
    void infoRightClicked(QMouseEvent *event);

    //Pure Virtual Functions
    virtual void propertiesOpen() = 0; //Children will call to open distinct properties windows
    virtual void saveTrack() = 0;
    virtual void saveTrackAs() = 0;

    //Miscilaneous Functions
    virtual void init();
    void updateSize();
    void setOffset(int offset);
    void setTrackChanged(bool trackChanged);
    void setSourceFile(QFile * sourceFile);
    void removeTrack();

signals:

public slots:
};

#endif // TRACK_H
