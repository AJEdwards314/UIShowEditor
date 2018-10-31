#include "serialdaemon.h"

#include <QSemaphore>
#include <QtSerialPort/QSerialPort>
#include <QDebug>

#include "point.h"

int SerialDaemon::current_id;

SerialDaemon::SerialDaemon(QObject * parent, SignalType signalType, QByteArray * payload, QSerialPort  * serialPort, QSemaphore * serialPortSem, int myId) : QThread(parent)
{
    this->signalType = signalType;
    this->payload = payload;
    this->message += mapping[signalType].header;
    this->message += *payload;
    this->message += mapping[signalType].header;
    this->serialPort = serialPort;
    this->serialPortSem = serialPortSem;
    this->myId = myId;
}

void SerialDaemon::run()
{
    serialPortSem->acquire();
    while(myId != current_id) {
        serialPortSem->release();
        QThread::yieldCurrentThread();
        serialPortSem->acquire();
    }
    if(signalType == STOP_RECORDING) {
        runStopRecording();
    } else {
        qInfo() << "Starting Message";
        serialPort->write(message);
        qInfo() << "Finishing Message";
        QString response = "";
        while(response.length() < 6) {
            char buffer[2];
            if(!serialPort->waitForReadyRead(1000))
                break; //Timed Out. TODO: Update with resend protocol
            serialPort->read(buffer, 2);
            response += buffer;
        }
        serialPort->clear();
    }
    current_id++;
    serialPortSem->release();
}

void SerialDaemon::runStopRecording()
{
    QList<Point> * points;
    qInfo() << "Getting Recording";
    serialPort->write(message);
    QString response = "";
    while(response.length() < 6) {
        char buffer[100];
        if(!serialPort->waitForReadyRead(-1))
            break;
        serialPort->read(buffer, 100);
        qInfo() << "Buffer: " << buffer;
        response += buffer;
    } //TODO: Fix this logic.  Should test rx string to know when to quit waiting for return.

    points = parseRecordingResponseString(response);
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
    int payloadLength = response.mid(2, 6).toInt(&ok1);
    if(!ok1)
        return nullptr;
    if(length < 2 + 6 + payloadLength + 2) // Header, size, payload, footer
        return nullptr;
    length = 2 + 6 + payloadLength + 2;
    if(response[0] != 'R' && response[1] != 'D' && response[length - 2] != 'R' && response[length - 1] != 'D')
        return nullptr;

    QString payload = response.mid(8, payloadLength);
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
