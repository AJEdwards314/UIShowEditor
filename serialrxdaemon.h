#ifndef SERIALDAEMON_PACKET_LENGTH
#define SERIALDAEMON_PACKET_LENGTH 1024
#endif

#ifndef SERIALRXDAEMON_H
#define SERIALRXDAEMON_H

#include "serialtxdaemon.h"

class QSerialPort;


class SerialRxDaemon : public QObject
{
    Q_OBJECT
public:
    SerialRxDaemon(QSerialPort * serialPort);

signals:
    void rxString(SerialTxDaemon::SignalType type, QString payload);

public slots:
    void readLine();

private:
    bool stopped;
    bool isInPacket;
    int bytesRemaining;
    QString packetHeader;
    QString packetFooter;
    QString packetBody;
    SerialTxDaemon::SignalType packetType;

    QSerialPort * serialPort;

    void startPacket(SerialTxDaemon::SignalType type, QString line);
    void fillPacket(QString line);
    void endPacket();
    void sendConnectionRx();
};

#endif // SERIALRXDAEMON_H
