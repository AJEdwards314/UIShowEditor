#ifndef PORTCONSTS_H
#define PORTCONSTS_H

#include <QStringList>

class PortConsts {
public:
    static const int numOutPorts = 10;
    static const int numInPorts = 8;
    static const QStringList outStrings;
    static const QStringList inStrings;
    static const QStringList colors;
};

#endif // PORTCONSTS_H
