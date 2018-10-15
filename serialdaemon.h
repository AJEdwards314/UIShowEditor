#ifndef SERIALDAEMON_H
#define SERIALDAEMON_H
#include <QThread>

struct Point;
class QSerialPort;
class QSemaphore;

class SerialDaemon : public QThread
{
public:
    enum SignalType {
        INCOMING_TRACK,
        INCOMING_SHOW,
        INCOMING_BEHAVIOR,
        START_PLAYBACK,
        PAUSE_PLAYBACK,
        STOP_PLAYBACK,
        CONFIGURE_RECORDING,
        START_RECORDING,
        STOP_RECORDING,
        RECORD_RXED
    };
    SerialDaemon(QObject *parent, SignalType signalType, QString payload, QSerialPort * serialPort, QSemaphore * serialPortSem);
    void run() override;
protected:
    struct HeaderMapping {
        SignalType type;
        char header[3];
    };
    const HeaderMapping mapping[10] = {
        {INCOMING_TRACK, "AD"},
        {INCOMING_SHOW, "SD"},
        {INCOMING_BEHAVIOR, "BD"},
        {START_PLAYBACK, "SP"},
        {PAUSE_PLAYBACK, "PP"},
        {STOP_PLAYBACK, "XP"},
        {CONFIGURE_RECORDING, "CR"},
        {START_RECORDING, "SR"},
        {STOP_RECORDING, "XR"},
        {RECORD_RXED, "RD"}
    };
    SignalType signalType;
    QString payload;
    QString message;
    QSerialPort * serialPort;
    QSemaphore * serialPortSem;

    void runStopRecording();
signals:
    void recordingReturned(QList<Point> points);
};

#endif // SERIALDAEMON_H
