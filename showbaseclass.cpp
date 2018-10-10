#include "showbaseclass.h"
#include <QTextStream>
#include <QRegularExpression>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include "showprimarypanel.h"
#include "track.h"
#include "ledtrack.h"
#include "motortrack.h"
#include "wavtrack.h"

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
    QList<Track*>* tracks = parentPanel->getTracks();
    for(int i = 0; i < tracks->length(); i++) {
        if(tracks->at(i)->getTrackChanged())
            tracks->at(i)->saveTrackParent();
    }

    sourceFile->open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(sourceFile);
    out << "Title," << title << "\n";
    out << "Tracks\n";
    for(int i = 0; i < tracks->length(); i++) {
        Track* track = tracks->at(i);
        out << track->getFilename() << ",";
        out << track->getOffset() << ",";
        out << track->getPort();
        LEDTrack* ledTrack;
        MotorTrack* motorTrack;
        if((ledTrack = dynamic_cast<LEDTrack*>(track))) {
            qInfo() << "LED Bar: " << ledTrack->getFilename();
            out << "," << ledTrack->getColorName();
        } else if((motorTrack = dynamic_cast<MotorTrack*>(track))) {
            qInfo() << "Motor Bar: " << motorTrack->getFilename();
            out << "," << (motorTrack->getReverse() ? 1 : 0);
        } else {
            qInfo() << "WAV Bar: " << track->getFilename();
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
