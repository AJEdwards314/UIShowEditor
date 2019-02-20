#ifndef PORTCONFIG_H
#define PORTCONFIG_H

#include <QFile>
#include <QList>

class PortConfig
{
public:
    static PortConfig * getInstance();
    static void setInstance();
    static void setInstance(QString filepath);

    static QString getType(QString port);
    static int getPortNum(QString port);

    struct OutConfigBase {
        QString type = "DOUT"; //SRV or DOUT
        int portNum = 1;
    };

    struct SRVConfig : OutConfigBase {
        int minVal = -127;
        int midVal = 0;
        int maxVal = 127;
        int reverse = 0;
    };
    struct LEDConfig : OutConfigBase {
        QString color = "Green";
    };
    struct InConfig {
        QString type; //AIN or DIN
        int portNum;
    };

    QList<OutConfigBase *> outConfigs;
    QList<InConfig *> inConfigs;

    void save();
    QStringList getPorts(QString type);
    QStringList getPorts(QStringList types);
    OutConfigBase * getOutputConfig(QString port);
    InConfig * getInputConfig(QString port);
    inline QFile * getSourceFile() {return sourceFile;}

private:
    PortConfig();
    PortConfig(QString filepath);
    QFile * sourceFile;
    static PortConfig * instance;

    void initNull();
    inline void setSourceFile(QString filepath) {sourceFile = new QFile(filepath); }
};

#endif // PORTCONFIG_H
