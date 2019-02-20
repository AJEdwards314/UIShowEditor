#include "controlleradapter.h"

#include <QFile>
#include <QFileInfo>
#include <QSemaphore>
#include <QDebug>
#include "point.h"
#include "showprimarypanel.h"
#include "serialrxdaemon.h"
#include "portreaddaemon.h"

ControllerAdapter * ControllerAdapter::instance;

ControllerAdapter::ControllerAdapter(QObject *parent) : QObject(parent)
{
    serialPort = new QSerialPort(this);
    serialPortSem = new QSemaphore(1);
    rxDaemon = new SerialRxDaemon(serialPort);
    connect(serialPort, &QIODevice::readyRead, rxDaemon, &SerialRxDaemon::readLine);
    connect(serialPort, &QSerialPort::errorOccurred, this, &ControllerAdapter::serialError);
}

ControllerAdapter * ControllerAdapter::getInstance()
{
    return instance;
}

SerialRxDaemon * ControllerAdapter::getRxDaemon()
{
    return rxDaemon;
}

void ControllerAdapter::setInstance(QObject * parent)
{
    instance = new ControllerAdapter(parent);
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
    return sendFile(SerialTxDaemon::INCOMING_TRACK, trackFile);
}

int ControllerAdapter::sendShow(QFile *showFile)
{
    return sendFile(SerialTxDaemon::INCOMING_SHOW, showFile);
}

int ControllerAdapter::sendBehavior(QFile *behaviorFile)
{
    return sendFile(SerialTxDaemon::INCOMING_BEHAVIOR, behaviorFile);
}

int ControllerAdapter::sendPortConfig(QFile * portConfigFile)
{
    return sendFile(SerialTxDaemon::INCOMING_PORT_CONFIG, portConfigFile);
}

int ControllerAdapter::startShow(QString filename)
{
    QByteArray * payload = new QByteArray();
    *payload += padFilename(filename);
    return startDaemon(SerialTxDaemon::START_PLAYBACK, payload);
}

int ControllerAdapter::pauseShow()
{
    QByteArray * payload = new QByteArray();
    return startDaemon(SerialTxDaemon::PAUSE_PLAYBACK, payload);
}

int ControllerAdapter::stopShow()
{
    QByteArray * payload = new QByteArray();
    return startDaemon(SerialTxDaemon::STOP_PLAYBACK, payload);
}

int ControllerAdapter::configureRecording(QString filename, QStringList * args)
{
    QString argString = "";
    for(int i = 0; i < args->length(); i++) {
        argString += args->at(i);
        argString += ",";
    }
    QByteArray * payload = new QByteArray();
    *payload += padFilename(filename);
    *payload += getLengthString(argString.length(), 4);
    *payload += argString;
    qInfo() << "In Configure Recording";
    return startDaemon(SerialTxDaemon::CONFIGURE_RECORDING, payload);
}

int ControllerAdapter::startRecording()
{
    QByteArray * payload = new QByteArray();
    return startDaemon(SerialTxDaemon::START_RECORDING, payload);
}

int ControllerAdapter::stopRecording(ShowPrimaryPanel * showPanel)
{
    //QList<Point> recordPoints;
    QByteArray * payload = new QByteArray();
    //*payload += "Stopping Recording";
    SerialTxDaemon * daemon;
    int returnVal = createDaemon(&daemon, SerialTxDaemon::STOP_RECORDING, payload);
    if(returnVal != 0)
        return 1;
    connect(daemon, &SerialTxDaemon::recordingReturned, showPanel, &ShowPrimaryPanel::recordingComplete);
    daemon->start();
    return returnVal;
}

int ControllerAdapter::drivePort(QString port, int val)
{
    QByteArray * payload = new QByteArray();
    SerialTxDaemon * daemon;
    QString argString = port + "," + QString::number(val);
    *payload += getLengthString(argString.length(), 4);
    *payload += argString;
    int returnVal = createDaemon(&daemon, SerialTxDaemon::DRIVE_PORT, payload);
    daemon->start();
    return returnVal;
}

int ControllerAdapter::killPort(QString port)
{
    QByteArray * payload = new QByteArray();
    SerialTxDaemon * daemon;
    QString argString = port + ",9999";
    *payload += getLengthString(argString.length(), 4);
    *payload += argString;
    int returnVal = createDaemon(&daemon, SerialTxDaemon::DRIVE_PORT, payload);
    daemon->start();
    return returnVal;
}

int ControllerAdapter::readPort(PortReadDaemon * portReadDaemon, QString port)
{
    QByteArray * payload = new QByteArray();
    QString argString = port;
    *payload += getLengthString(argString.length(), 4);
    *payload += port;
    SerialTxDaemon * daemon;
    int returnVal = createDaemon(&daemon, SerialTxDaemon::READ_PORT, payload);
    if(returnVal != 0)
        return 1;
    connect(rxDaemon, &SerialRxDaemon::rxString, portReadDaemon, &PortReadDaemon::respRxed);
    //connect(daemon, &SerialTxDaemon::valueReturned, dialog, &PortTestDialog::readReturned);
    daemon->start();
    return returnVal;
}

int ControllerAdapter::createDaemon(SerialTxDaemon ** daemon, SerialTxDaemon::SignalType signalType, QByteArray * payload, QByteArray * dataPayload)
{
    if(!(serialPort->isOpen() && serialPort->isWritable() && serialPort->isReadable()))
        return 1;
    *daemon = new SerialTxDaemon(this, signalType, payload, serialPort, serialPortSem, getnextId(), dataPayload);
    return 0;
}

int ControllerAdapter::startDaemon(SerialTxDaemon::SignalType signalType, QByteArray * payload, QByteArray * dataPayload)
{
    //qInfo() << "In start daemon";
    SerialTxDaemon *daemon;
    if(createDaemon(&daemon, signalType, payload, dataPayload) != 0)
        return 1;
    daemon->start();
    //qInfo() << "End start daemon";
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
    //qInfo() << *outArr;
    file->close();
    return outArr;
}

QString ControllerAdapter::getLengthString(int length, int padLength)
{
    return QString("%1").arg(length, padLength, 10, QChar('0'));
}

int ControllerAdapter::sendFile(SerialTxDaemon::SignalType signalType, QFile *file)
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

void ControllerAdapter::respRxed(SerialTxDaemon::SignalType type, QString payload) {
    if(type != SerialTxDaemon::SHOW_FINISHED) {
        return;
    }
    emit stopShow();
}

void ControllerAdapter::serialError(QSerialPort::SerialPortError error) {
    qInfo() << "Serial Port Error: " << error;
    if(error == QSerialPort::UnsupportedOperationError)
        serialPort->clear(QSerialPort::Output);
}

