#ifndef SERIALDAEMON_PACKET_LENGTH
#define SERIALDAEMON_PACKET_LENGTH 1024
#endif

#ifndef SERIALDAEMON_H
#define SERIALDAEMON_H
#include <QThread>

struct Point;
class QSerialPort;
class QSemaphore;

class SerialTxDaemon : public QThread
{
    Q_OBJECT
public:
    static int current_id;

    enum SignalType {
        INCOMING_TRACK,
        INCOMING_SHOW,
        INCOMING_BEHAVIOR,
        INCOMING_PORT_CONFIG,
        START_PLAYBACK,
        PAUSE_PLAYBACK,
        STOP_PLAYBACK,
        CONFIGURE_RECORDING,
        START_RECORDING,
        STOP_RECORDING,
        RECORD_RXED,
        PACKET,
        DRIVE_PORT,
        READ_PORT,
        PACKET_ACK,
        SERIAL_CHECK,
        SHOW_FINISHED,
        ERROR
    };
    SerialTxDaemon(QObject *parent, SignalType signalType, QByteArray * payload, QSerialPort * serialPort, QSemaphore * serialPortSem, int myId, QByteArray * dataPayload = nullptr);
    void run() override;
    static SignalType getSignalType(QString typeString);

public slots:
    void readReturned(SignalType type, QString payload);

protected:
    struct HeaderMapping {
        SignalType type;
        QString header;
    };
    static const HeaderMapping mapping[ERROR];
    int myId;
    long long myTime;

    SignalType signalType;
    QByteArray * payload;
    QByteArray * dataPayload;
    QByteArray message;
    QSerialPort * serialPort;
    QSemaphore * serialPortSem;

    void runStopRecording();
    QList<Point> * parseRecordingResponseString(QString response);
    void runNoReply();

    static const int SHORT_TIME_OUT = 100;
    static const int MID_TIME_OUT   = 1000;
    static const int LONG_TIME_OUT  = 10000;

    bool waitingForRead;
    bool waitingForPktAck;
    bool pktAck;
    QString readPayload;

    void waitForRead(int timeout);
    void waitForPacketAck(int timeout);
    void getPacketAck(QString payload);


signals:
    void recordingReturned(QList<Point> * points);
    void valueReturned(int val);
};

#endif // SERIALDAEMON_H
