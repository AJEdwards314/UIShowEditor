#define WAV_SHOW_PNT_DENSITY 100

#ifndef WAVTRACK_H
#define WAVTRACK_H

#include <QWidget>
#include "track.h"
class QFile;

class WAVTrack : public Track
{
public:
    WAVTrack(QWidget *parent = nullptr, float pixpersec = 100, QFile * sourceFile = nullptr, int offset = 0, QString port = "AUD1");
    void apply(int offset, QString port);
protected:
    //Attributes
    qint16 chanels;
    qint32 sampleRate;
    qint16 bytesPerSample;
    int absMax = 0;
    bool needs_repaint;
    QList<qint16> samples;
    QPixmap pixmap;

    //From QWidget
    void paintEvent(QPaintEvent *event) override;

    //From Track
    void propertiesOpen() override;
    void saveTrack() override;
    void saveTrackAs() override;

    //Other Functions
    void repaintWAV();
};

#endif // WAVSHOWBARWIDGET_H
