#define MOTOR_TRACK_POINT_RAD 2

#ifndef MOTORTRACK_H
#define MOTORTRACK_H

#include "track.h"
class QFile;
struct Point;

class MotorTrack : public Track
{
public:
    explicit MotorTrack(QWidget *parent = nullptr, float pixpersec = 100, QFile * sourceFile = nullptr, int offset = 0, QString port = "SRV1", bool reverse = false);
    explicit MotorTrack(QWidget *parent = nullptr, float pixpersec = 100, QStringList * args = nullptr, QList<Point> * points = nullptr);

    void apply(QString name, int offset, QString port); //Called by properties type window

    inline bool getReverse() {return reverse;}

protected:
    //Attributes
    QList<Point> points;
    int maxVal;
    int minVal;
    int midVal;
    bool reverse;

    //From QWidget
    void paintEvent(QPaintEvent *event) override;

    //From Track
    void init() override;
    void propertiesOpen() override;
    void saveTrack() override;
    void saveTrackAs() override;

    //Other Functions
    void setLimits();
};

#endif // MOTORTRACK_H
