#define SHOW_BAR_HANDLE_WIDTH 10
#define SHOW_BAR_INFO_WIDTH 150
#define SHOW_BAR_HEIGHT 55
#ifndef SHOWBARWIDGET_H
#define SHOWBARWIDGET_H



#include <QWidget>

class QFile;
class ShowPrimaryPanel;

class ShowBarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ShowBarWidget(QWidget *parent = nullptr, float pixpersec = 100, int offset = 0, QFile *sourceFile = nullptr, QString port = "");
    void setScale(float pixpersec, int width);
    int getBaseWidth();
    inline QString getFilename() {return filename;}
    inline int getOffset() {return offset;}
    inline QString getPort() {return port;}
    void saveTrackParent();
    void saveTrackAsParent();
    inline bool getTrackChanged() {return trackChanged;}
protected:
    ShowPrimaryPanel * parentPanel;
    void paintEvent(QPaintEvent *) override;
    int index;
    int offset;
    QString port;
    int length;
    int idealWidth;
    float augmentedLength;
    float augmentedOffset;
    QString title;
    QFile *sourceFile;
    QString filename;
    void mousePressEvent(QMouseEvent *event) override;
    void handleClicked(QMouseEvent *event);
    void infoRightClicked(QMouseEvent *event);
    void drawHandle(QPainter &painter);
    QRectF handle, infoBox;
    void updateSize();
    float pixpersec;
    virtual void propertiesOpen(); //Children will call to open distinct properties windows
    void setOffset(int offset);
    virtual void saveTrack() = 0;
    virtual void saveTrackAs() = 0;
    bool trackChanged = false;
    void setTrackChanged(bool trackChanged);
    void setSourceFile(QFile * sourceFile);
    void removeTrack();

signals:

public slots:
};

#endif // SHOWBARWIDGET_H
