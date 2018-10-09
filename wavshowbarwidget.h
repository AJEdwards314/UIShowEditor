#define WAV_SHOW_PNT_DENSITY 100

#ifndef WAVSHOWBARWIDGET_H
#define WAVSHOWBARWIDGET_H

#include <QWidget>
#include "showbarwidget.h"
class QFile;

class WAVShowBarWidget : public ShowBarWidget
{
public:
    WAVShowBarWidget(QWidget *parent = nullptr, float pixpersec = 100, QFile * sourceFile = nullptr, int offset = 0, QString port = "AUD1");
    void apply(int offset, QString port);
protected:
    void paintEvent(QPaintEvent *event) override;
    void repaintWAV();
    qint16 chanels;
    qint32 sampleRate;
    qint16 bytesPerSample;
    int absMax = 0;
    QList<qint16> samples;
    QPixmap pixmap;
    bool needs_repaint;
    void propertiesOpen() override;
    void saveTrack() override;
    void saveTrackAs() override;
};

#endif // WAVSHOWBARWIDGET_H
