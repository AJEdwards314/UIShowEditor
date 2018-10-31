#ifndef CONTROLLERADAPTER_H
#define CONTROLLERADAPTER_H

#include <QObject>
#include <QSemaphore>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "serialdaemon.h"

class QFile;
class QSerialPort;
class ShowPrimaryPanel;
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
    int startShow(QString filename);
    int pauseShow();
    int stopShow();
    int configureRecording(QString filename, QStringList* args);
    int startRecording();
    int stopRecording(ShowPrimaryPanel * showPanel);

protected:
    QSemaphore * serialPortSem;
    QSerialPort * serialPort;
    QSerialPortInfo serialPortInfo;

    int createDaemon(SerialDaemon ** daemon, SerialDaemon::SignalType signalType, QByteArray * payload);
    int startDaemon(SerialDaemon::SignalType signalType, QByteArray * payload);
    QString getFilename(QFile *file);
    QString padFilename(QString filename);
    QByteArray * readFile(QFile *file);
    QString getLengthString(int length);


    int sendFile(SerialDaemon::SignalType signalType, QFile *file);
    int getnextId();

    int id = -1;
signals:

public slots:
};

#endif // CONTROLLERADAPTER_H
