#define MOTOR_SHOW_BAR_POINT_RAD 2

#ifndef MOTORSHOWBARWIDGET_H
#define MOTORSHOWBARWIDGET_H

#include "showbarwidget.h"
class QFile;

class MotorShowBarWidget : public ShowBarWidget
{
public:
    explicit MotorShowBarWidget(QWidget *parent = nullptr, float pixpersec = 100, QFile * sourceFile = nullptr, int offset = 0, QString port = "SRV1", bool reverse = false);
    void apply(QString name, int offset, QString port, int maxVal, int minVal, int defVal, bool reverse); //Called by properties type window
    inline bool getReverse() {return reverse;}
protected:
    void paintEvent(QPaintEvent *event) override;
    int maxVal;
    int minVal;
    int midVal;
    bool reverse;
    struct Point {
        int val;
        int ms;
    };
    QList<Point> points;
    void propertiesOpen() override;
    void saveTrack() override;
    void saveTrackAs() override;
};

#endif // MOTORSHOWBARWIDGET_H
