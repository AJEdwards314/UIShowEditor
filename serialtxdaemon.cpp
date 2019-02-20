#include "serialtxdaemon.h"

#include <QSemaphore>
#include <QtSerialPort/QSerialPort>
#include <QDebug>
#include <QCryptographicHash>
#include <QDateTime>

#include "point.h"
#include "controlleradapter.h"
#include "serialrxdaemon.h"

int SerialTxDaemon::current_id;

const SerialTxDaemon::HeaderMapping SerialTxDaemon::mapping[ERROR]  = {
    {INCOMING_TRACK, "AD"},
    {INCOMING_SHOW, "SD"},
    {INCOMING_BEHAVIOR, "BD"},
    {INCOMING_PORT_CONFIG, "PD"},
    {START_PLAYBACK, "SP"},
    {PAUSE_PLAYBACK, "PP"},
    {STOP_PLAYBACK, "XP"},
    {CONFIGURE_RECORDING, "CR"},
    {START_RECORDING, "SR"},
    {STOP_RECORDING, "XR"},
    {RECORD_RXED, "RD"},
    {PACKET, "PK"},
    {DRIVE_PORT, "DP"},
    {READ_PORT, "RP"},
    {PACKET_ACK, "KP"},
    {SERIAL_CHECK, "SC"},
    {SHOW_FINISHED, "SF"}
};

SerialTxDaemon::SerialTxDaemon(QObject * parent, SignalType signalType, QByteArray * payload, QSerialPort  * serialPort, QSemaphore * serialPortSem, int myId, QByteArray * dataPayload) : QThread(parent)
{
    //qInfo() << "Init Thread";
    this->signalType = signalType;
    this->payload = payload;
    this->dataPayload = dataPayload;
    this->message += mapping[signalType].header;
    this->message += *payload;
    this->myTime = QDateTime::currentMSecsSinceEpoch();

    if(dataPayload != nullptr) {
        QCryptographicHash hash(QCryptographicHash::Sha1);
        hash.addData(*dataPayload);
        this->message += hash.result();
    }

    this->message += mapping[signalType].header;
    this->serialPort = serialPort;
    this->serialPortSem = serialPortSem;
    this->myId = myId;
    qInfo() << "Thread " << QString::number(myId) << ": Created";
}

void SerialTxDaemon::run()
{
    //qInfo() << "Thread " << QString::number(myId) << ": Running";
    bool hasDataPayload = signalType == INCOMING_TRACK || signalType == INCOMING_SHOW || signalType == INCOMING_BEHAVIOR || signalType == INCOMING_PORT_CONFIG;
    serialPortSem->acquire();
    while(myId != current_id) {
        serialPortSem->release();
        QThread::yieldCurrentThread();
        serialPortSem->acquire();
    }
    qInfo() << "Thread " << QString::number(myId) << ": Running";
    serialPort->clear(QSerialPort::Output);
    //qInfo() << "Current Time: " << QString::number(QDateTime::currentMSecsSinceEpoch()) << ", My Time: " + QString::number(myTime);
    if(QDateTime::currentMSecsSinceEpoch() > myTime + MID_TIME_OUT) {
        qInfo() << "Thread " << QString::number(myId) << ": Expired";
    } else if(signalType == STOP_RECORDING) {
        runStopRecording();
    } else if(signalType == READ_PORT || signalType == DRIVE_PORT) {
        runNoReply();
    } else {
        serialPort->write(message);
        serialPort->flush();

        qInfo() << "Thread " << QString::number(myId) << ": Waiting";
        waitForRead(SHORT_TIME_OUT);
        if(readPayload[2] == 'X') //Do not send file
            hasDataPayload = false;

        if(hasDataPayload) {
            int bytesRemaining = dataPayload->length();
            int index = 0;
            while(bytesRemaining > 0) {
                int bytesToSend = qMin(bytesRemaining, SERIALDAEMON_PACKET_LENGTH);
                QByteArray packet;
                packet += mapping[PACKET].header;
                packet += dataPayload->mid(index, bytesToSend);
                packet += mapping[PACKET].header;
                serialPort->write(packet);
                serialPort->flush();
                waitForPacketAck(MID_TIME_OUT);
                if(!pktAck)
                    qt_noop(); //TODO Throw Error and resend packet or something
                index += bytesToSend;
                bytesRemaining -= bytesToSend;
                //qInfo() << bytesRemaining;
            }
        }

    }
    serialPort->clear(QSerialPort::Input);
    qInfo() << "Finishing Message";

    current_id++;
    serialPortSem->release();
    qInfo() << "End Run Thread";
}

SerialTxDaemon::SignalType SerialTxDaemon::getSignalType(QString typeString) {
    for(int i = 0; i < SignalType::ERROR; i++) {
        if(typeString == mapping[i].header)
            return mapping[i].type;
    }
    return SignalType::ERROR;
}

void SerialTxDaemon::runStopRecording()
{
    QList<Point> * points;
    //qInfo() << "Getting Recording";
    serialPort->write(message);
    serialPort->flush();
    waitForRead(SHORT_TIME_OUT);
    //serialPort->clear(QSerialPort::Input);
    //qInfo() << "Response = " << readPayload;

    this->signalType = RECORD_RXED;
    waitForRead(LONG_TIME_OUT);
    /*QString dataResponse = "";
    char buffer[2];
    bool rRead = false;
    int numTagsRead = 0;
    buffer[1] = '\0';
    i = 0;
    while (numTagsRead < 2) {
        int numRead = serialPort->read(buffer, 1);
        if(numRead == 0) {
            this->msleep(1);
            i++;
            i++;
            if(i > timeOutMs) {
                qInfo() << "Request Timed Out: Returning";
                return;
            }
            continue;
        }
        qInfo() << buffer;
        dataResponse += buffer;
        if(rRead && buffer[0] == 'D')
            numTagsRead++;
        rRead = buffer[0] == 'R';
    }*/

    points = parseRecordingResponseString(readPayload);
    if(points == nullptr)
        return; //TODO: Error occured.  Resend or something
    emit recordingReturned(points);
}

void SerialTxDaemon::runNoReply()
{
    //qInfo() << "Sending no reply";
    serialPort->write(message);
    serialPort->flush();
}

QList<Point> * SerialTxDaemon::parseRecordingResponseString(QString response)
{
    //qInfo() << response;
    bool ok1, ok2;
    int length = response.length();
    if(length < 10)
        return nullptr;
    int payloadLength = response.mid(2, 10).toInt(&ok1);
    if(!ok1)
        return nullptr;
    if(length < 2 + 10 + payloadLength + 2) // Header, size, payload, footer
        return nullptr;
    length = 2 + 10 + payloadLength + 2;
    if(response[0] != 'R' && response[1] != 'D' && response[length - 2] != 'R' && response[length - 1] != 'D')
        return nullptr;

    QString payload = response.mid(12, payloadLength);
    QStringList pointStrings = payload.split(";");

    QList<Point> * points = new QList<Point>;
    for(int i = 0; i < pointStrings.length(); i++) {
        QStringList pointVals = pointStrings[i].split(",");
        points->append({pointVals[0].toInt(&ok1), pointVals[1].toInt(&ok2)});
        if(!ok1 || !ok2) {
            delete points;
            return nullptr;
        }
    }
    return points;
}

void SerialTxDaemon::readReturned(SignalType type, QString payload) {
    //qInfo() << "Serial Tx Daemon Received: " + payload;
    if(type == PACKET_ACK) {
        getPacketAck(payload);
        return;
    }
    //qInfo() << "Type: " + QString::number(type) + "  Expected: " + QString::number(signalType);
    if(type != this->signalType)
        return;
    readPayload = payload;
    waitingForRead = false;
}

void SerialTxDaemon::waitForRead(int timeout) {
    connect(ControllerAdapter::getInstance()->getRxDaemon(), &SerialRxDaemon::rxString, this, &SerialTxDaemon::readReturned);
    int i = 0;
    waitingForRead = true;
    while(waitingForRead) {
        msleep(1);
        i++;
        if(timeout >= 0 && i > timeout) {
            qInfo() << "Request Timed Out: Returning";
            return;
        }
    }
    disconnect(ControllerAdapter::getInstance()->getRxDaemon(), &SerialRxDaemon::rxString, this, &SerialTxDaemon::readReturned);
}

void SerialTxDaemon::waitForPacketAck(int timeout) {
    connect(ControllerAdapter::getInstance()->getRxDaemon(), &SerialRxDaemon::rxString, this, &SerialTxDaemon::readReturned);
    waitingForPktAck = true;
    pktAck = false;
    int i = 0;
    while(waitingForPktAck) {
        msleep(1);
        i++;
        if(timeout >= 0 && i > timeout) {
            qInfo() << "Request Timed Out: Returning";
            return;
        }
    }
    disconnect(ControllerAdapter::getInstance()->getRxDaemon(), &SerialRxDaemon::rxString, this, &SerialTxDaemon::readReturned);
}

void SerialTxDaemon::getPacketAck(QString payload) {
    if(payload.length() >= 3)
        pktAck = payload[2] == 'A';
    waitingForPktAck = false;
}
