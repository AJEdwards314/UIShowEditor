#include "portreaddaemon.h"

#include "porttestdialog.h"
#include "controlleradapter.h"

PortReadDaemon::PortReadDaemon(PortTestDialog * parent, QString port)
{
    this->parentDialog = parent;
    this->port = port;
    stopped = false;
    connect(parentDialog, &PortTestDialog::closeRead, this, &PortReadDaemon::closePort);
    connect(this, &PortReadDaemon::valueReturned, parentDialog, &PortTestDialog::readReturned);
}

/*PortReadDaemon::~PortReadDaemon() {

}*/

void PortReadDaemon::run()
{
    while(!stopped) {
        ControllerAdapter::getInstance()->readPort(this, port);
        msleep(periodMs);
    }
    ControllerAdapter::getInstance()->killPort(port);
}

void PortReadDaemon::respRxed(SerialTxDaemon::SignalType type, QString payload) {
    if(type != SerialTxDaemon::READ_PORT)
        return;
    emit valueReturned(payload.mid(2,5).toInt());
}


void PortReadDaemon::closePort()
{

    disconnect(parentDialog, &PortTestDialog::closeRead, this, &PortReadDaemon::closePort);
    disconnect(this, &PortReadDaemon::valueReturned, parentDialog, &PortTestDialog::readReturned);
    stopped = true;
}
