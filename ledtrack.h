#define LED_TRACK_POINT_RAD 2

#ifndef LEDTRACK_H
#define LEDTRACK_H

#include "track.h"
class QFile;
struct Point;

class LEDTrack : public Track
{
public:
    LEDTrack(QWidget *parent = nullptr, float pixpersec = 100, QFile * sourceFile = nullptr, int offset = 0, QString port = "DOUT1");
    LEDTrack(QWidget *parent = nullptr, float pixpersec = 100, QStringList * args = nullptr, QList<Point> * points = nullptr);

    void apply(QString name, int offset, QString port); //Called by properties type window

    inline QString getColorName() {return colorName;}
protected:
    //Attributes
    QList<Point> points;
    QString colorName;
    QColor color;

    //From QWidget
    void paintEvent(QPaintEvent *event) override;

    //From Track
    void init() override;
    void propertiesOpen() override;
    void saveTrack() override;
    void saveTrackAs() override;

    //Internal Functions
    void setColor();
};

#endif // LEDTRACK_H
