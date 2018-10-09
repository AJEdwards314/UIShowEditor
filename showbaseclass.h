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
    QString title;
    void save();
    void saveAs(QFile *newSourceFile);
    inline QString getFilename() {return filename;}
protected:
    ShowPrimaryPanel * parentPanel;
    void setSourceFile(QFile *sourceFile);
    QFile *sourceFile;
    QString filename;



signals:

public slots:
};

#endif // SHOWBASECLASS_H
