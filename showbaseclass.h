#ifndef SHOWBASECLASS_H
#define SHOWBASECLASS_H

#include <QObject>
class QFile;
class ShowPrimaryPanel;

class ShowBaseClass : public QObject
{
    Q_OBJECT
public:
    explicit ShowBaseClass(QObject *parent = nullptr, QString filename = ""); //Parent here will be a ShowPrimaryPanel

    //Saving functionality
    void save();
    void saveAs(QFile *newSourceFile);

    //Accessors
    inline QString getFilename() {return filename;}
    inline QFile * getFile() {return sourceFile;}
protected:
    //Key References
    ShowPrimaryPanel * parentPanel;
    QFile *sourceFile;

    //Attributes
    QString title;
    QString filename;

    //Functions
    void setSourceFile(QFile *sourceFile);

signals:

public slots:
};

#endif // SHOWBASECLASS_H
