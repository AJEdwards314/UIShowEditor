#define LED_SHOW_BAR_POINT_RAD 2

#ifndef LEDSHOWBARWIDGET_H
#define LEDSHOWBARWIDGET_H

#include "showbarwidget.h"
class QFile;

class LEDShowBarWidget : public ShowBarWidget
{
public:
    LEDShowBarWidget(QWidget *parent = nullptr, float pixpersec = 100, QFile * sourceFile = nullptr, int offset = 0, QString port = "DIO1", QString colorName = "Green");
    void apply(QString name, int offset, QString port, QString colorName); //Called by properties type window
    inline QString getColorName() {return colorName;}
protected:
    void paintEvent(QPaintEvent *event) override;
    QString colorName;
    QColor color;
    struct Point {
        bool val;
        int ms;
    };
    QList<Point> points;
    void propertiesOpen() override;
    void setColor(QString colorName);
    void saveTrack() override;
    void saveTrackAs() override;
};

#endif // LEDSHOWBARWIDGET_H
