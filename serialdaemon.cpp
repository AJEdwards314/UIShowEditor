#include "serialdaemon.h"

#include <QSemaphore>
#include <QtSerialPort/QSerialPort>

SerialDaemon::SerialDaemon(QObject * parent, SignalType signalType, QString payload, QSerialPort  * serialPort, QSemaphore * serialPortSem) : QThread(parent)
{
    this->signalType = signalType;
    this->payload = payload;
    this->message = mapping[signalType].header + payload + mapping[signalType].header;
    this->serialPort = serialPort;
    this->serialPortSem = serialPortSem;
}

void SerialDaemon::run()
{
    serialPortSem->acquire();
    if(signalType == STOP_RECORDING) {
        runStopRecording();
    } else {
        serialPort->write(message.toUtf8());
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
    serialPortSem->release();
}

void SerialDaemon::runStopRecording()
{
    //emit recordingReturned(points);
}
