#define SERIALDAEMON_PACKET_LENGTH 4096

#ifndef SERIALDAEMON_H
#define SERIALDAEMON_H
#include <QThread>

struct Point;
class QSerialPort;
class QSemaphore;

class SerialDaemon : public QThread
{
    Q_OBJECT
public:
    static int current_id;

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
        RECORD_RXED,
        PACKET
    };
    SerialDaemon(QObject *parent, SignalType signalType, QByteArray * payload, QSerialPort * serialPort, QSemaphore * serialPortSem, int myId, QByteArray * dataPayload = nullptr);
    void run() override;

protected:
    struct HeaderMapping {
        SignalType type;
        char header[3];
    };
    const HeaderMapping mapping[11] = {
        {INCOMING_TRACK, "AD"},
        {INCOMING_SHOW, "SD"},
        {INCOMING_BEHAVIOR, "BD"},
        {START_PLAYBACK, "SP"},
        {PAUSE_PLAYBACK, "PP"},
        {STOP_PLAYBACK, "XP"},
        {CONFIGURE_RECORDING, "CR"},
        {START_RECORDING, "SR"},
        {STOP_RECORDING, "XR"},
        {RECORD_RXED, "RD"},
        {PACKET, "PK"}
    };
    int myId;

    SignalType signalType;
    QByteArray * payload;
    QByteArray * dataPayload;
    QByteArray message;
    QSerialPort * serialPort;
    QSemaphore * serialPortSem;

    void runStopRecording();
    QList<Point> * parseRecordingResponseString(QString response);


signals:
    void recordingReturned(QList<Point> * points);
};

#endif // SERIALDAEMON_H
