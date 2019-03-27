#include "portconfig.h"

#include <QTextStream>
#include <QRegularExpression>
#include "portconsts.h"

PortConfig * PortConfig::instance;

PortConfig * PortConfig::getInstance() {
    return instance;
}

void PortConfig::setInstance() {
    instance = new PortConfig();
}

void PortConfig::setInstance(QString filepath) {
    instance = new PortConfig(filepath);
}

QString PortConfig::getType(QString port) {
    QRegularExpression regex("([A-Z]+)([0-9]+)");
    QRegularExpressionMatch match = regex.match(port);
    return match.captured(1);
}

int PortConfig::getPortNum(QString port) {
    QRegularExpression regex("([A-Z]+)([0-9]+)");
    QRegularExpressionMatch match = regex.match(port);
    return match.captured(2).toInt();
}

PortConfig::PortConfig()
{
    initNull();
}

PortConfig::PortConfig(QString filepath)
{
    setSourceFile(filepath);
    if(!sourceFile->open(QIODevice::ReadOnly | QIODevice::Text)) {
        initNull();
        return;
    }

    QTextStream in(sourceFile);
    QString line;
    QStringList list;

    for(int i = 0; i < PortConsts::numOutPorts; i++) {
        line = in.readLine();
        list = line.split(QRegularExpression(","));
        QString type = list[0];
        int portNum = list[1].toInt();
        OutConfigBase * config;
        if(type == "SRV") {
            SRVConfig * srvConfig = new SRVConfig;
            srvConfig->minVal = list[2].toInt();
            srvConfig->midVal = list[3].toInt();
            srvConfig->maxVal = list[4].toInt();
            srvConfig->reverse = list[5].toInt();
            config = srvConfig;
        } else if(type == "DOUT") {
            LEDConfig * ledConfig = new LEDConfig;
            ledConfig->color = list[2];
            config = ledConfig;
        } else {
            LEDConfig * ledConfig = new LEDConfig;
            ledConfig->type = "DOUT";
            ledConfig->portNum = i + 1;
            ledConfig->color = "Green";
            config = ledConfig;
        }
        config->type = type;
        config->portNum = portNum;
        outConfigs.append(config);
    }

    for(int i = 0; i < PortConsts::numInPorts; i++) {
        line = in.readLine();
        list = line.split(QRegularExpression(","));
        QString type = list[0];
        int portNum = list[1].toInt();
        InConfig * config = new InConfig;
        config->type = type;
        config->portNum = portNum;
        inConfigs.append(config);
    }
    sourceFile->close();
}

void PortConfig::save() {
    sourceFile->open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(sourceFile);
    for(int i = 0; i < PortConsts::numOutPorts; i++) {
        out << outConfigs[i]->type << "," << QString::number(outConfigs[i]->portNum) << ",";
        if(outConfigs[i]->type == "SRV") {
            SRVConfig * srvConfig = (SRVConfig *) outConfigs[i];
            out <<  QString::number(srvConfig->minVal) << "," <<
                    QString::number(srvConfig->midVal) << "," <<
                    QString::number(srvConfig->maxVal) << "," <<
                    (srvConfig->reverse ? "1" : "0");
        } else {
            LEDConfig * ledConfig = (LEDConfig *) outConfigs[i];
            out << ledConfig->color;
        }
        out << "\n";
    }

    for(int i = 0; i < PortConsts::numInPorts; i++) {
        out << inConfigs[i]->type << "," << QString::number(inConfigs[i]->portNum) << "\n";
    }
    sourceFile->close();
}

QStringList PortConfig::getPorts(QString type) {
    QStringList types;
    if(type == "OUT") {
        types.append("SRV");
        types.append("DOUT");
        types.append("AUD");
    } else if (type == "IN") {
        types.append("AIN");
        types.append("DIN");
        types.append("VOIC");
    } else {
        types.append(type);
    }
    return getPorts(types);
}

QStringList PortConfig::getPorts(QStringList types) {
    QStringList outList;


    for(int i = 0;  i < outConfigs.length(); i++) {
        if(types.contains(outConfigs[i]->type)) {
            outList.append(outConfigs[i]->type + QString::number(outConfigs[i]->portNum));
        }
    }
    for(int i = 0;  i < inConfigs.length(); i++) {
        if(types.contains(inConfigs[i]->type)) {
            outList.append(inConfigs[i]->type + QString::number(inConfigs[i]->portNum));
        }
    }
    if(types.contains("AUD")) {
        outList.append("AUD1");
    }
    if(types.contains("VOIC")) {
        outList.append("VOIC1");
    }
    return outList;
}

PortConfig::OutConfigBase * PortConfig::getOutputConfig(QString port)
{
    //QRegularExpression regex("([A-Z]+)([0-9]+)");
    //QRegularExpressionMatch match = regex.match(port);
    //QString type = match.captured(1);
    //int portNum = match.captured(2).toInt();

    QString type = getType(port);
    int portNum = getPortNum(port);
    if(outConfigs[portNum - 1]->type == type)
        return outConfigs[portNum - 1];
    if(type == "SRV") {
        SRVConfig * config = new SRVConfig;
        config->type = "SRV";
        config->portNum = portNum;
        return config;
    } else {
        LEDConfig * config = new LEDConfig;
        config->type = "LED";
        config->portNum = portNum;
        return config;
    }
}

PortConfig::InConfig * PortConfig::getInputConfig(QString port)
{
    QRegularExpression regex("([A-Z]+)([0-9]+)");
    QRegularExpressionMatch match = regex.match(port);
    QString type = match.captured(1);
    int portNum = match.captured(2).toInt();
    if(inConfigs[portNum -1]->type == type)
        return inConfigs[portNum - 1];
    InConfig * config = new InConfig;
    config->type = type;
    config->portNum = portNum;
    return config;
}





void PortConfig::initNull()
{
    outConfigs.clear();
    for(int i = 0; i < PortConsts::numOutPorts; i++) {
        LEDConfig * config = new LEDConfig;
        config->type = "DOUT";
        config->portNum = i + 1;
        config->color = "Green";
        outConfigs.append(config);
    }
    for(int i = 0; i < PortConsts::numInPorts; i++) {
        InConfig * config = new InConfig;
        config->type = "AIN";
        config->portNum = i + 1;
        inConfigs.append(config);
    }
}
