#define MOTOR_TRACK_POINT_RAD 2

#ifndef MOTORTRACK_H
#define MOTORTRACK_H

#include "track.h"
class QFile;

class MotorTrack : public Track
{
public:
    explicit MotorTrack(QWidget *parent = nullptr, float pixpersec = 100, QFile * sourceFile = nullptr, int offset = 0, QString port = "SRV1", bool reverse = false);

    void apply(QString name, int offset, QString port, int maxVal, int minVal, int defVal, bool reverse); //Called by properties type window

    inline bool getReverse() {return reverse;}

protected:
    struct Point {
        int val;
        int ms;
    };

    //Attributes
    QList<Point> points;
    int maxVal;
    int minVal;
    int midVal;
    bool reverse;

    //From QWidget
    void paintEvent(QPaintEvent *event) override;

    //From Track
    void propertiesOpen() override;
    void saveTrack() override;
    void saveTrackAs() override;
};

#endif // MOTORTRACK_H
