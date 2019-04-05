#ifndef CONTROLLERADAPTER_H
#define CONTROLLERADAPTER_H

#include <QObject>
#include <QSemaphore>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "serialtxdaemon.h"
#include "porttestdialog.h"

class SerialRxDaemon;
class PortReadDaemon;
class QFile;
class QSerialPort;
class ShowPrimaryPanel;
struct Point;

class ControllerAdapter : public QObject
{
    Q_OBJECT
public:
    static ControllerAdapter * getInstance();

    SerialRxDaemon * getRxDaemon();

    static void setInstance(QObject * parent = nullptr);

    int startSerialConnection(QString port, QSerialPort::BaudRate baud);
    void stopSerialConnection();

    int transferShow(QFile *showFile);
    int transferBehavior(QFile *behaviorFile);
    int sendTrack(QFile *trackFile);
    int sendShow(QFile *showFile);
    int sendBehavior(QFile *behaviorFile);
    int sendPortConfig(QFile * portConfigFile);
    int startShow(QString filename);
    int pauseShow();
    int stopShow();
    int configureRecording(QString filename, QStringList* args);
    int startRecording();
    int stopRecording(ShowPrimaryPanel * showPanel);
    int drivePort(QString port, int val);
    int killPort(QString port);
    int readPort(PortReadDaemon * portReadDaemon, QString port);

private:
    explicit ControllerAdapter(QObject *parent = nullptr);
    static ControllerAdapter * instance;

//signals:
//    void closePort();

protected:
    QSemaphore * serialPortSem;
    QSerialPort * serialPort;
    QSerialPortInfo serialPortInfo;
    SerialRxDaemon * rxDaemon;

    int createDaemon(SerialTxDaemon ** daemon, SerialTxDaemon::SignalType signalType, QByteArray * payload, QByteArray * dataPayload = nullptr);
    int startDaemon(SerialTxDaemon::SignalType signalType, QByteArray * payload, QByteArray * dataPayload = nullptr);
    QString getFilename(QFile *file);
    QString padFilename(QString filename);
    QByteArray * readFile(QFile *file);
    QString getLengthString(int length, int padLength = 10);


    int sendFile(SerialTxDaemon::SignalType signalType, QFile *file, QString filename = "");

    int getnextId();

    int id = -1;
signals:
    void stopPlayback();
public slots:
    void respRxed(SerialTxDaemon::SignalType type, QString payload);
    void serialError(QSerialPort::SerialPortError error);
};

#endif // CONTROLLERADAPTER_H
