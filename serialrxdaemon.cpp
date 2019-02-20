#include "serialrxdaemon.h"

#include <QSerialPort>
#include <QDebug>

SerialRxDaemon::SerialRxDaemon(QSerialPort * serialPort)
{
    this->serialPort = serialPort;
}

void SerialRxDaemon::readLine()
{
    while(serialPort->canReadLine()) {
        QString line = serialPort->readLine();
        SerialTxDaemon::SignalType type = SerialTxDaemon::getSignalType(line.mid(0,2));
        QString payload = line.trimmed();
        qInfo() << "Read: " << line;
        //qInfo() << "Packet Type: " << type;
        if(type == SerialTxDaemon::RECORD_RXED)
            startPacket(type, payload);
        else if(type == SerialTxDaemon::PACKET)
            fillPacket(payload);
        else if(type == SerialTxDaemon::SERIAL_CHECK)
            sendConnectionRx();
        else {
            //qInfo() << "Emit Called (" + QString::number(type) + ", " + payload + ")";
            emit rxString(type, payload);
        }
    }
}

void SerialRxDaemon::startPacket(SerialTxDaemon::SignalType type, QString line) {
    isInPacket = true;
    packetType = type;
    bytesRemaining = line.mid(2, 10).toInt();
    packetHeader = line.mid(0, 2 + 10);
    packetFooter = line.mid(line.length() - 2, 2);
    packetBody = "";
    QString header = line.mid(0,2);
    serialPort->write("RDARD");
    serialPort->flush();
}

void SerialRxDaemon::fillPacket(QString line) {
    QString trimmedLine = line.mid(2, line.length() - 4);
    bytesRemaining -= trimmedLine.length();
    packetBody += trimmedLine;
    serialPort->write("KPAKP");
    serialPort->flush();
    //qInfo() << "Bytes Remaining: " + QString::number(bytesRemaining);
    if(bytesRemaining <= 0)
        endPacket();
}

void SerialRxDaemon::endPacket() {
    QString packet = packetHeader + packetBody + packetFooter;
    //qInfo() << "Total Packet: " + packet;
    //qInfo() << "Emit Called (" + QString::number(packetType) + ", " + packet + ")";
    emit rxString(packetType, packet);
}

void SerialRxDaemon::sendConnectionRx() {
    serialPort->write("SCSC");
    serialPort->flush();
}
