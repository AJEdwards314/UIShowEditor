#include "serialdaemon.h"

#include <QSemaphore>
#include <QtSerialPort/QSerialPort>
#include <QDebug>
#include <QCryptographicHash>

#include "point.h"

int SerialDaemon::current_id;

SerialDaemon::SerialDaemon(QObject * parent, SignalType signalType, QByteArray * payload, QSerialPort  * serialPort, QSemaphore * serialPortSem, int myId, QByteArray * dataPayload) : QThread(parent)
{
    qInfo() << "Init Thread";
    this->signalType = signalType;
    this->payload = payload;
    this->dataPayload = dataPayload;
    this->message += mapping[signalType].header;
    this->message += *payload;

    if(dataPayload != nullptr) {
        QCryptographicHash hash(QCryptographicHash::Sha1);
        hash.addData(*dataPayload);
        this->message += hash.result();
    }

    this->message += mapping[signalType].header;
    this->serialPort = serialPort;
    this->serialPortSem = serialPortSem;
    this->myId = myId;
    qInfo() << "End Init Thread";
}

void SerialDaemon::run()
{
    qInfo() << "Run Thread";
    bool hasDataPayload = signalType == INCOMING_TRACK || signalType == INCOMING_SHOW || signalType == INCOMING_BEHAVIOR;
    serialPortSem->acquire();
    while(myId != current_id) {
        serialPortSem->release();
        QThread::yieldCurrentThread();
        serialPortSem->acquire();
    }
    qInfo() << "Sem Acquired";
    if(signalType == STOP_RECORDING) {
        runStopRecording();
    } else {
        qInfo() << "Starting Message";
        serialPort->write(message);
        serialPort->flush();
        //serialPort->waitForReadyRead(-1);
        while(serialPort->bytesAvailable()<5) {msleep(1);}
        char response[6];
        serialPort->read(response, 5);
        serialPort->clear(QSerialPort::Input);
        if(response[2] == 'X') //Do not send file
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
                //serialPort->waitForReadyRead(-1);
                while(serialPort->bytesAvailable()<5) {msleep(1);}
                char response[6];
                serialPort->read(response, 5);
                serialPort->clear(QSerialPort::Input);
                if(response != "PKAPK")
                    qt_noop(); //TODO Throw Error and resend packet or something
                index += bytesToSend;
                bytesRemaining -= bytesToSend;
                qInfo() << bytesRemaining;
            }
        }

    }
    serialPort->clear(QSerialPort::Input);
    qInfo() << "Finishing Message";

    current_id++;
    serialPortSem->release();
    qInfo() << "End Run Thread";
}

void SerialDaemon::runStopRecording()
{
    QList<Point> * points;
    qInfo() << "Getting Recording";
    serialPort->write(message);
    serialPort->flush();
    //serialPort->waitForReadyRead(-1);
    while(serialPort->bytesAvailable()<5) {msleep(1);}
    char response[6];
    serialPort->read(response, 5);
    //serialPort->clear(QSerialPort::Input);
    qInfo() << "Response = " << response;



    QString dataResponse = "";
    char buffer[2];
    bool rRead = false;
    int numTagsRead = 0;
    buffer[1] = '\0';
    while (numTagsRead < 2) {
        int numRead = serialPort->read(buffer, 1);
        if(numRead == 0) {
            this->msleep(1);
            continue;
        }
        qInfo() << buffer;
        dataResponse += buffer;
        if(rRead && buffer[0] == 'D')
            numTagsRead++;
        rRead = buffer[0] == 'R';
    }

    points = parseRecordingResponseString(dataResponse);
    if(points == nullptr)
        return; //TODO: Error occured.  Resend or something
    emit recordingReturned(points);
}

QList<Point> * SerialDaemon::parseRecordingResponseString(QString response)
{
    qInfo() << response;
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
