#ifndef WORKINGDIRECTORY_H
#define WORKINGDIRECTORY_H

#include <QString>
#include <QDir>

class WorkingDirectory
{
public:
    static void setDir(QString dirString);
    static QDir getDir();
    static QString getPath();
private:
    WorkingDirectory();
    static QDir dir;

};

#endif // WORKINGDIRECTORY_H
