#include "controlleradapter.h"

#include <QFile>
#include <QFileInfo>
#include <QSemaphore>
#include <QDebug>
#include "point.h"

ControllerAdapter::ControllerAdapter(QObject *parent) : QObject(parent)
{
    serialPort = new QSerialPort(this);
    serialPortSem = new QSemaphore(1);
}

int ControllerAdapter::startSerialConnection(QString port, QSerialPort::BaudRate baud)
{
    serialPort->setPortName(port);
    serialPort->open(QIODevice::ReadWrite);
    serialPort->setBaudRate(baud);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::HardwareControl);

    qInfo() << "Is Open: " << serialPort->isOpen();
    qInfo() << "Is Writable: " << serialPort->isWritable();
    qInfo() << "Is Readable: " << serialPort->isReadable();

    if(!(serialPort->isOpen() && serialPort->isWritable() && serialPort->isReadable()))
        return 1;
    return 0;
}

void ControllerAdapter::stopSerialConnection()
{
     serialPort->close();
}

int ControllerAdapter::sendTrack(QFile *trackFile)
{
    return sendFile(SerialDaemon::INCOMING_TRACK, trackFile);
}

int ControllerAdapter::sendShow(QFile *showFile)
{
    return sendFile(SerialDaemon::INCOMING_SHOW, showFile);
}

int ControllerAdapter::sendBehavior(QFile *behaviorFile)
{
    return sendFile(SerialDaemon::INCOMING_BEHAVIOR, behaviorFile);
}

int ControllerAdapter::startShow(QFile *showFile)
{
    QString filename = getFilename(showFile);
    return startDaemon(SerialDaemon::START_PLAYBACK, filename);
}

int ControllerAdapter::pauseShow()
{
    return startDaemon(SerialDaemon::PAUSE_PLAYBACK, "");
}

int ControllerAdapter::stopShow()
{
    return startDaemon(SerialDaemon::STOP_PLAYBACK, "");
}

int ControllerAdapter::configureRecording(QFile *showFile, QString portName, QList<QString> args)
{
    QString filename = getFilename(showFile);
    QString argString = "";
    for(int i = 0; i < args.length(); i++)
        argString += args[i];
    QString payload = filename + portName + argString;
    return startDaemon(SerialDaemon::CONFIGURE_RECORDING, payload);
}

int ControllerAdapter::startRecording()
{
    return startDaemon(SerialDaemon::START_RECORDING, "");
}

int ControllerAdapter::stopRecording(QList<Point> ** outPoints)
{
    QList<Point> recordPoints;
    int returnVal = startDaemon(SerialDaemon::STOP_RECORDING, "Stopping Recording");
    return returnVal;
    //TODO Read in returned point vals to recordPoints
}

int ControllerAdapter::startDaemon(SerialDaemon::SignalType signalType, QString payload)
{
    if(!(serialPort->isOpen() && serialPort->isWritable() && serialPort->isReadable()))
        return 1;
    SerialDaemon *daemon = new SerialDaemon(this, signalType, payload, serialPort, serialPortSem);
    daemon->start();
    return 0;
}

QString ControllerAdapter::getFilename(QFile *file)
{
    if(file == nullptr)
        return "";
    return QFileInfo(*file).fileName().leftJustified(40, ' ');
}

QString ControllerAdapter::readFile(QFile *file)
{
    QString outString;
    if(file == nullptr)
        return "";
    file->open(QIODevice::ReadOnly | QFile::Text);
    if(!(file->isOpen() && file->isReadable()))
        return "";

    QTextStream stream(file);
    outString = stream.readAll();
    file->close();
    return outString;
}

QString ControllerAdapter::getLengthString(QString filedata)
{
    return QString("%1").arg(filedata.length(), 6, 10, QChar('0'));
}

int ControllerAdapter::sendFile(SerialDaemon::SignalType signalType, QFile *file)
{
    QString filename = getFilename(file);
    QString filedata = readFile(file);
    if(filename == "" || filedata == "")
        return 1;
    QString filelength = getLengthString(filedata);

    QString payload = filename + filelength + filedata;
    return startDaemon(signalType, payload);
}

