#ifndef PORTREADDAEMON_H
#define PORTREADDAEMON_H

#include<QThread>

#include "serialtxdaemon.h"

class PortTestDialog;

class PortReadDaemon : public QThread
{
Q_OBJECT
public:
    PortReadDaemon(PortTestDialog * parent, QString port);
    //~PortReadDaemon() override;
    void run() override;

public slots:
    void closePort();
    void respRxed(SerialTxDaemon::SignalType type, QString payload);

signals:
    void valueReturned(int val);

private:
    static const int periodMs = 100;
    QString port;
    PortTestDialog * parentDialog;
    bool stopped = false;

};

#endif // PORTREADDAEMON_H
