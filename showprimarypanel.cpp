#include "showprimarypanel.h"

#include <QLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QPainter>
#include <QCoreApplication>
#include <QApplication>
#include <QtGlobal>
#include <QFileInfo>
#include <QDragMoveEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QRandomGenerator>

#include "track.h"
#include "motortrack.h"
#include "ledtrack.h"
#include "showbaseclass.h"
#include "showeditorwindow.h"
#include "wavtrack.h"
#include "controlleradapter.h"
#include "point.h"
#include "portconfig.h"
#include "workingdirectory.h"


ShowPrimaryPanel::ShowPrimaryPanel(QWidget *parent) : QWidget(parent)
{
    //this->setMouseTracking(true);
    this->setAcceptDrops(true);
    //this->setMinimumWidth(1500);
    tracks = new QList<Track*>();
    parentWindow = (ShowEditorWindow*)parent;
    showBase = nullptr;
    new QVBoxLayout(this); //Creates Vertical Layout for Panel

    updateTitle();
}

void ShowPrimaryPanel::openShow(QString filename)
{
    newShow();
    QFile file(filename);
    this->showBase = new ShowBaseClass(this, filename);
    generateShowSavedRv();
    updateTitle();
}

void ShowPrimaryPanel::createEmptyShow()
{
    this->showBase = new ShowBaseClass(this);
}

void ShowPrimaryPanel::newShow()
{
    if(showChanged) {
        QMessageBox msgBox;
        msgBox.setText("The document has been modified.");
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();
        switch(ret) {
        case QMessageBox::Save:
            save();
            break;
        case QMessageBox::Cancel:
            return;
            break;
        }
    }

    for(int i = 0; i < tracks->length(); i++)  //Remove all currently loaded Tracks
    {
        layout()->removeWidget(tracks->at(i));
        tracks->at(i)->hide();
    }
    tracks->clear();
    showBase = nullptr;
    showChanged = false;
    updateTitle();
}

void ShowPrimaryPanel::openTracks(QStringList &filenames, QList<int> * offsets, QList<QString> * ports, QList<QList<QString>> *args)
{
    for(int i = 0; i < filenames.length(); i++)
    {
        QFileInfo fileinfo(filenames[i]);
        QFile *file = new QFile(filenames[i]);
        Track *track = nullptr;
        //TODO add file extension error checking
        int offset = 0;
        QString port = "";
        if(offsets != nullptr)
            offset = offsets->at(i);
        if(ports != nullptr)
            port = ports->at(i);
        else {
            if(fileinfo.suffix() == "lsr") {
                QStringList portList = PortConfig::getInstance()->getPorts("DOUT");
                if(portList.length() == 0)
                    port = "DOUT1";
                else
                    port = portList[0];
            } else if(fileinfo.suffix() == "osr") {
                QStringList portList = PortConfig::getInstance()->getPorts("SRV");
                if(portList.length() == 0)
                    port = "SRV1";
                else
                    port = portList[0];
            } else if(fileinfo.suffix() == "wav") {
                QStringList portList = PortConfig::getInstance()->getPorts("AUD");
                if(portList.length() == 0)
                    port = "AUD1";
                else
                    port = portList[0];
            } else {
                port = "SRV1";
            }
        }


        QList<QString> currentArgs;
        if(args != nullptr)
            currentArgs = args->at(i);
        if(fileinfo.suffix() == "lsr") {
            track = new LEDTrack(this, pixpersec, file, offset, port);
        } else if(fileinfo.suffix() == "osr") {
            if(currentArgs.length() == 0)
                track = new MotorTrack(this, pixpersec, file, offset, port);
            else {
                bool reverse = currentArgs[0] != "0";
                track = new MotorTrack(this, pixpersec, file, offset, port, reverse);
            }
        } else {
            track = new WAVTrack(this, pixpersec, file, offset, port); //TODO .WAV
        }
        layout()->addWidget(track);
        tracks->append(track);
        updateChildren();
        trackShowDataUpdated();
    }
    update();

}

void ShowPrimaryPanel::newTrack(QStringList * recordingArgs, QList<Point> * points)
{
    Track * track;
    if(recordingArgs->at(0) == "MOT") //Motor Track
        track = new MotorTrack(this, pixpersec, recordingArgs, points);
    else if(recordingArgs->at(0) == "LED")
        track = new LEDTrack(this, pixpersec, recordingArgs, points);
    layout()->addWidget(track);
    tracks->append(track);

    QMessageBox msgBox;
    msgBox.setText("Please Save the New Track");
    msgBox.exec();

    track->saveTrackAsParent();
    updateChildren();
}

void ShowPrimaryPanel::trackShowDataUpdated()
{
    if(showBase != nullptr) {
        showChanged = true;
        updateTitle();
    }
}

void ShowPrimaryPanel::removeTrack(Track *track)
{
    layout()->removeWidget(track);
    tracks->removeAll(track);
    track->hide();
    showChanged = true;
}

bool ShowPrimaryPanel::save()
{
    if(!hasShow()) {
        createEmptyShow();
        return saveAs();
    }

    showBase->save();
    showChanged = false;
    generateShowSavedRv();
    updateTitle();
    return true;
}

bool ShowPrimaryPanel::saveAs()
{
    if(!hasShow()) {
        createEmptyShow();
    }

    QString filepath = QFileDialog::getSaveFileName(this, tr("Save Show"),WorkingDirectory::getPath(),tr("Animaniacs Show Files (*.shw)"));
    if(filepath == "")
        return false;
    QFile * newSourceFile = new QFile(filepath);

    showBase->saveAs(newSourceFile);
    showChanged = false;
    generateShowSavedRv();
    updateTitle();
    return true;
}

void ShowPrimaryPanel::updateTitle()
{
    if(showBase != nullptr) {
        if(showChanged)
            parentWindow->setWindowTitle(showBase->getFilename() + "* - Animaniacs Show Builder");
        else
            parentWindow->setWindowTitle(showBase->getFilename() + " - Animaniacs Show Builder");
    } else {
        parentWindow->setWindowTitle("unsaved - Animaniacs Show Builder");
    }
}

void ShowPrimaryPanel::objectGrabbed(Track* widget)
{
    grabbedTrack = widget;
    originalSlot = tracks->indexOf(widget);
    currentSlot = originalSlot;
    tracks->removeOne(widget);
}


bool ShowPrimaryPanel::transferShow()
{
    if(!hasShow()) {
        if(!save())
            return false;
    }

    if(showSavedRv == showTransferedRv)
        return true;

    ControllerAdapter::getInstance()->transferShow(showBase->getFile());

    /*
    for(int i = 0; i < tracks->length(); i++)
        ControllerAdapter::getInstance()->sendTrack(tracks->at(i)->getFile());
    ControllerAdapter::getInstance()->sendShow(showBase->getFile());
    ControllerAdapter::getInstance()->sendPortConfig(PortConfig::getInstance()->getSourceFile());
    */

    showTransferedRv = showSavedRv;
    return true;
}

void ShowPrimaryPanel::startShow()
{
    if(!hasShow())
        save();
    ControllerAdapter::getInstance()->startShow(showBase->getFilename());
}

void ShowPrimaryPanel::pauseShow()
{
    ControllerAdapter::getInstance()->pauseShow();
}

void ShowPrimaryPanel::stopShow()
{
    ControllerAdapter::getInstance()->stopShow();
}

void ShowPrimaryPanel::configureRecording(QStringList* args)
{
    if(!hasShow())
        save();
    recordingArgs = new QStringList(*args);
    ControllerAdapter::getInstance()->configureRecording(showBase->getFilename(), args);
}

void ShowPrimaryPanel::startRecording()
{
    ControllerAdapter::getInstance()->startRecording();
}

void ShowPrimaryPanel::stopRecording()
{
    QList<Point> * pointList;
    ControllerAdapter::getInstance()->stopRecording(this);
}


void ShowPrimaryPanel::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if(currentSlot != -1) {
        QRectF rect;
        if(currentSlot == 0)
            rect = QRectF(0, 0, grabbedTrack->width(), tracks->at(currentSlot)->pos().y());
        else if(currentSlot == tracks->length())
            rect = QRectF(0, tracks->at(currentSlot - 1)->geometry().bottomRight().y(), grabbedTrack->width(), height() - tracks->at(currentSlot - 1)->geometry().bottomRight().y());
        else
            rect = QRectF(0, tracks->at(currentSlot - 1)->geometry().bottomRight().y(), grabbedTrack->width(), tracks->at(currentSlot)->pos().y() - tracks->at(currentSlot - 1)->geometry().bottomRight().y());
        painter.setBrush(Qt::magenta);
        painter.drawRect(rect);
    }// else {
    //    QRectF size(0, 0, this->width(), this->height());
    //    painter.setBrush(QColor(255, 255, 240));
    //    painter.setPen(Qt::transparent);
    //    painter.drawRect(size);
    //}
}

void ShowPrimaryPanel::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
}

void ShowPrimaryPanel::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
    currentSlot = calculateSlot(event->pos());
    repaint();
}

void ShowPrimaryPanel::dropEvent(QDropEvent *event)
{
    event->acceptProposedAction();
    if(currentSlot == -1)
        currentSlot = originalSlot;
    placeAtSlot(grabbedTrack, currentSlot);
    grabbedTrack = nullptr;
    currentSlot = -1;
    originalSlot = -1;
    repaint();
}

int ShowPrimaryPanel::calculateSlot(QPoint p)
{
    for(int i = 0; i <= tracks->length(); i++) {
        if(i == 0) {
            if(p.y() < tracks->at(i)->pos().y())
                return i;
        } else if(i == tracks->length()) {
            if(p.y() > tracks->at(i-1)->geometry().bottomRight().y())
                return i;
        } else {
            if(p.y() < tracks->at(i)->pos().y() && p.y() > tracks->at(i-1)->geometry().bottomRight().y())
                return i;
        }
    }
    return -1;
}

void ShowPrimaryPanel::placeAtSlot(Track *track, int index)
{
    for(int i = index; i < tracks->length(); i++)
    {
        layout()->removeWidget(tracks->at(i));
    }
    track->setVisible(true);
    tracks->insert(index, track);
    for(int i = index; i < tracks->length(); i++) {
        layout()->addWidget(tracks->at(i));
    }
}

void ShowPrimaryPanel::wheelEvent(QWheelEvent *event)
{
    if(QApplication::queryKeyboardModifiers() & Qt::ControlModifier)
        event->accept();
    else
        return;

    if(event->delta() > 0)
        pixpersec *= 1.5;
    else
        pixpersec /= 1.5;
    updateChildren();
}

void ShowPrimaryPanel::updateChildren()
{
    for(int i = 0; i < tracks->length(); i++) {
        tracks->at(i)->setScale(pixpersec, 0);
    }
    int maxWidth = 0;
    for(int i = 0; i < tracks->length(); i++) {
        maxWidth = qMax(tracks->at(i)->getBaseWidth(), maxWidth);

    }
    for(int i = 0; i < tracks->length(); i++) {
        tracks->at(i)->setScale(pixpersec, maxWidth);
    }
}

void ShowPrimaryPanel::generateShowSavedRv()
{
    showSavedRv = QRandomGenerator::global()->generate();
}

void ShowPrimaryPanel::recordingComplete(QList<Point> * points)
{
    for(int i = 0; i < points->length(); i++)
        qInfo() << points->at(i).val << ", " << points->at(i).ms;
    newTrack(recordingArgs, points);
}






