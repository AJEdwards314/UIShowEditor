#include "showbaseclass.h"
#include <QTextStream>
#include <QRegularExpression>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include "showprimarypanel.h"
#include "showbarwidget.h"
#include "ledshowbarwidget.h"
#include "motorshowbarwidget.h"
#include "wavshowbarwidget.h"

ShowBaseClass::ShowBaseClass(QObject *parent, QString filepath) : QObject(parent)
{
    parentPanel = (ShowPrimaryPanel*) parent;
    setSourceFile(new QFile(filepath));
    QFileInfo fileinfo(filepath);
    if(filepath != "") {
        this->filename = fileinfo.fileName();
        if(!sourceFile->open(QIODevice::ReadOnly | QIODevice::Text))
            return; //Todo throw file not found error

        QTextStream in(sourceFile);
        QString line;
        QStringList list;

        line = in.readLine();
        list = line.split(QRegularExpression(","));
        if(list[0] != "Title") {
            qInfo("Title Not Found");
            return; //TODO File Corrupted
        }
        title = list[1];

        line = in.readLine();
        if(line != "Tracks") {
            qInfo("Tracks Not Found");
            return; //TODO File Corrupted
        }
        QStringList trackFiles;
        QList<int> trackOffsets;
        QList<QString> trackPorts;
        QList<QList<QString>> fullArgs;
        while((line = in.readLine()) != "EndTracks") {
            list = line.split(QRegularExpression(","));
            trackFiles.append(fileinfo.absolutePath() + "/" + list[0]);
            qInfo() << fileinfo.absolutePath();
            trackOffsets.append(list[1].toInt());
            trackPorts.append(list[2]);
            QList<QString> args;
            for(int i = 3; i < list.length(); i++) {
                args.append(list[i]);
            }
            fullArgs.append(args);
        }
        sourceFile->close();
        parentPanel->openTracks(trackFiles, &trackOffsets, &trackPorts, &fullArgs);
    } else {
        filename = "";
        title = "Untitled";
    }
}

void ShowBaseClass::save()
{
    QList<ShowBarWidget*>* showBars = parentPanel->getShowBars();
    for(int i = 0; i < showBars->length(); i++) {
        if(showBars->at(i)->getTrackChanged())
            showBars->at(i)->saveTrackParent();
    }

    sourceFile->open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(sourceFile);
    out << "Title," << title << "\n";
    out << "Tracks\n";
    for(int i = 0; i < showBars->length(); i++) {
        ShowBarWidget* showBar = showBars->at(i);
        out << showBar->getFilename() << ",";
        out << showBar->getOffset() << ",";
        out << showBar->getPort();
        LEDShowBarWidget* ledShowBar;
        MotorShowBarWidget* motorShowBar;
        if(ledShowBar = dynamic_cast<LEDShowBarWidget*>(showBar)) {
            qInfo() << "LED Bar: " << ledShowBar->getFilename();
            out << "," << ledShowBar->getColorName();
        } else if(motorShowBar = dynamic_cast<MotorShowBarWidget*>(showBar)) {
            qInfo() << "Motor Bar: " << motorShowBar->getFilename();
            out << "," << (motorShowBar->getReverse() ? 1 : 0);
        } else {
            qInfo() << "WAV Bar: " << showBar->getFilename();
        }
        out << "\n";
    }
    out << "EndTracks";
    sourceFile->close();
}

void ShowBaseClass::saveAs(QFile *newSourceFile) {
    setSourceFile(newSourceFile);
    save();
    parentPanel->updateTitle();
}

void ShowBaseClass::setSourceFile(QFile *sourceFile) {
    this->sourceFile = sourceFile;
    this->filename = QFileInfo(*sourceFile).fileName();
}
