#ifndef CONTROLLERADAPTER_H
#define CONTROLLERADAPTER_H

#include <QObject>
#include <QSemaphore>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "serialdaemon.h"

class QFile;
class QSerialPort;
struct Point;

class ControllerAdapter : public QObject
{
    Q_OBJECT
public:
    explicit ControllerAdapter(QObject *parent = nullptr);
    int startSerialConnection(QString port, QSerialPort::BaudRate baud);
    void stopSerialConnection();

    int sendTrack(QFile *trackFile);
    int sendShow(QFile *showFile);
    int sendBehavior(QFile *behaviorFile);
    int startShow(QFile *showFile);
    int pauseShow();
    int stopShow();
    int configureRecording(QFile *showFile, QString portName, QList<QString> args);
    int startRecording();
    int stopRecording(QList<Point> ** outPoints);

protected:
    QSemaphore * serialPortSem;
    QSerialPort * serialPort;
    QSerialPortInfo serialPortInfo;

    int startDaemon(SerialDaemon::SignalType signalType, QString payload);
    QString getFilename(QFile *file);
    QString readFile(QFile *file);
    QString getLengthString(QString filebody);

    int sendFile(SerialDaemon::SignalType signalType, QFile *file);


signals:

public slots:
};

#endif // CONTROLLERADAPTER_H
