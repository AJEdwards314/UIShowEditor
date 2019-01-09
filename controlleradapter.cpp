#include "controlleradapter.h"

#include <QFile>
#include <QFileInfo>
#include <QSemaphore>
#include <QDebug>
#include "point.h"
#include "showprimarypanel.h"

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
    serialPort->setFlowControl(QSerialPort::NoFlowControl);

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

int ControllerAdapter::startShow(QString filename)
{
    QByteArray * payload = new QByteArray();
    *payload += padFilename(filename);
    return startDaemon(SerialDaemon::START_PLAYBACK, payload);
}

int ControllerAdapter::pauseShow()
{
    QByteArray * payload = new QByteArray();
    return startDaemon(SerialDaemon::PAUSE_PLAYBACK, payload);
}

int ControllerAdapter::stopShow()
{
    QByteArray * payload = new QByteArray();
    return startDaemon(SerialDaemon::STOP_PLAYBACK, payload);
}

int ControllerAdapter::configureRecording(QString filename, QStringList * args)
{
    QString argString = "";
    for(int i = 0; i < args->length(); i++)
        argString += args->at(i);
        argString += ",";
    QByteArray * payload = new QByteArray();
    *payload += padFilename(filename);
    *payload += getLengthString(argString.length(), 4);
    *payload += argString;
    qInfo() << "In Configure Recording";
    return startDaemon(SerialDaemon::CONFIGURE_RECORDING, payload);
}

int ControllerAdapter::startRecording()
{
    QByteArray * payload = new QByteArray();
    return startDaemon(SerialDaemon::START_RECORDING, payload);
}

int ControllerAdapter::stopRecording(ShowPrimaryPanel * showPanel)
{
    QList<Point> recordPoints;
    QByteArray * payload = new QByteArray();
    //*payload += "Stopping Recording";
    SerialDaemon * daemon;
    int returnVal = createDaemon(&daemon, SerialDaemon::STOP_RECORDING, payload);
    if(returnVal != 0)
        return 1;
    connect(daemon, &SerialDaemon::recordingReturned, showPanel, &ShowPrimaryPanel::recordingComplete);
    daemon->start();
    return returnVal;
}

int ControllerAdapter::createDaemon(SerialDaemon ** daemon, SerialDaemon::SignalType signalType, QByteArray * payload, QByteArray * dataPayload)
{
    if(!(serialPort->isOpen() && serialPort->isWritable() && serialPort->isReadable()))
        return 1;
    *daemon = new SerialDaemon(this, signalType, payload, serialPort, serialPortSem, getnextId(), dataPayload);
    return 0;
}

int ControllerAdapter::startDaemon(SerialDaemon::SignalType signalType, QByteArray * payload, QByteArray * dataPayload)
{
    qInfo() << "In start daemon";
    SerialDaemon *daemon;
    if(createDaemon(&daemon, signalType, payload, dataPayload) != 0)
        return 1;
    daemon->start();
    qInfo() << "End start daemon";
    return 0;
}

QString ControllerAdapter::getFilename(QFile *file)
{
    if(file == nullptr)
        return "";
    return padFilename(QFileInfo(*file).fileName());
}

QString ControllerAdapter::padFilename(QString filename)
{
    return filename.leftJustified(40, ' ');
}

QByteArray * ControllerAdapter::readFile(QFile *file)
{
    QByteArray * outArr = new QByteArray;
    if(file == nullptr)
        return outArr;
    file->open(QIODevice::ReadOnly);
    if(!(file->isOpen() && file->isReadable()))
        return outArr;

    *outArr = file->readAll();
    file->close();
    return outArr;
}

QString ControllerAdapter::getLengthString(int length, int padLength)
{
    return QString("%1").arg(length, padLength, 10, QChar('0'));
}

int ControllerAdapter::sendFile(SerialDaemon::SignalType signalType, QFile *file)
{
    QString filename = getFilename(file);
    QByteArray * filedata = readFile(file);
    if(filename == "" || filedata->length() == 0)
        return 1;
    QString filelength = getLengthString(filedata->length());

    QByteArray *payload = new QByteArray;
    *payload += filename.toUtf8();
    *payload += filelength.toUtf8();
    //*payload += *filedata;
    return startDaemon(signalType, payload, filedata);
}

int ControllerAdapter::getnextId() {
    id++;
    return id;
}

