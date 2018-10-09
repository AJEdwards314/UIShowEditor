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
#include "showbarwidget.h"
#include "motorshowbarwidget.h"
#include "ledshowbarwidget.h"
#include "showbaseclass.h"
#include "showeditorwindow.h"
#include "wavshowbarwidget.h"


ShowPrimaryPanel::ShowPrimaryPanel(QWidget *parent) : QWidget(parent)
{
    //this->setMouseTracking(true);
    this->setAcceptDrops(true);
    //this->setMinimumWidth(1500);
    showBars = new QList<ShowBarWidget*>();
    parentWindow = (ShowEditorWindow*)parent;
    showBase = nullptr;
    new QVBoxLayout(this); //Creates Vertical Layout for Panel
    /*for(int i = 0; i < 10; i++) {
        ShowBarWidget *bar =  new ShowBarWidget(this, pixpersec, i, "abcdef", i*1000);
        layout()->addWidget(bar);
        showBars->append(bar);
    }*/

    /*QFile file("C:\\Users\\alex\\OneDrive\\Documents\\Systems\\UI Test\\Test Files\\TestWav.wav");
    ShowBarWidget *bar = new WAVShowBarWidget(this,pixpersec, &file);
    layout()->addWidget(bar);
    showBars->append(bar);
    updateChildren();*/
    /*QFile file("C:\\Users\\alex\\OneDrive\\Documents\\Systems\\UI Test\\Test Files\\test_track.osr");
    ShowBarWidget *bar = new MotorShowBarWidget(this, pixpersec, &file);
    layout()->addWidget(bar);
    showBars->append(bar);

    QFile file2("C:\\Users\\alex\\OneDrive\\Documents\\Systems\\UI Test\\Test Files\\test_led_track.lsr");
    bar = new LEDShowBarWidget(this, pixpersec, &file2);
    layout()->addWidget(bar);
    showBars->append(bar);
    updateChildren();*/
    updateTitle();
}

void ShowPrimaryPanel::objectGrabbed(ShowBarWidget* widget)
{
    grabbedShowBar = widget;
    originalSlot = showBars->indexOf(widget);
    currentSlot = originalSlot;
    showBars->removeOne(widget);
}

void ShowPrimaryPanel::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
    currentSlot = calculateSlot(event->pos());
    repaint();
}

int ShowPrimaryPanel::calculateSlot(QPoint p)
{
    for(int i = 0; i <= showBars->length(); i++) {
        if(i == 0) {
            if(p.y() < showBars->at(i)->pos().y())
                return i;
        } else if(i == showBars->length()) {
            if(p.y() > showBars->at(i-1)->geometry().bottomRight().y())
                return i;
        } else {
            if(p.y() < showBars->at(i)->pos().y() && p.y() > showBars->at(i-1)->geometry().bottomRight().y())
                return i;
        }
    }
    return -1;
}

void ShowPrimaryPanel::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
}

void ShowPrimaryPanel::dropEvent(QDropEvent *event)
{
    event->acceptProposedAction();
    if(currentSlot == -1)
        currentSlot = originalSlot;
    placeAtSlot(grabbedShowBar, currentSlot);
    grabbedShowBar = nullptr;
    currentSlot = -1;
    originalSlot = -1;
    repaint();
}

void ShowPrimaryPanel::placeAtSlot(ShowBarWidget *showBar, int index)
{
    for(int i = index; i < showBars->length(); i++)
    {
        layout()->removeWidget(showBars->at(i));
    }
    showBar->setVisible(true);
    showBars->insert(index, showBar);
    for(int i = index; i < showBars->length(); i++) {
        layout()->addWidget(showBars->at(i));
    }
}

void ShowPrimaryPanel::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if(currentSlot != -1) {
        QRectF rect;
        if(currentSlot == 0)
            rect = QRectF(0, 0, grabbedShowBar->width(), showBars->at(currentSlot)->pos().y());
        else if(currentSlot == showBars->length())
            rect = QRectF(0, showBars->at(currentSlot - 1)->geometry().bottomRight().y(), grabbedShowBar->width(), height() - showBars->at(currentSlot - 1)->geometry().bottomRight().y());
        else
            rect = QRectF(0, showBars->at(currentSlot - 1)->geometry().bottomRight().y(), grabbedShowBar->width(), showBars->at(currentSlot)->pos().y() - showBars->at(currentSlot - 1)->geometry().bottomRight().y());
        painter.setBrush(Qt::magenta);
        painter.drawRect(rect);
    }// else {
    //    QRectF size(0, 0, this->width(), this->height());
    //    painter.setBrush(QColor(255, 255, 240));
    //    painter.setPen(Qt::transparent);
    //    painter.drawRect(size);
    //}
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
    for(int i = 0; i < showBars->length(); i++) {
        showBars->at(i)->setScale(pixpersec, 0);
    }
    int maxWidth = 0;
    for(int i = 0; i < showBars->length(); i++) {
        maxWidth = qMax(showBars->at(i)->getBaseWidth(), maxWidth);

    }
    for(int i = 0; i < showBars->length(); i++) {
        showBars->at(i)->setScale(pixpersec, maxWidth);
    }
}

void ShowPrimaryPanel::openShow(QString filename)
{
    newShow();
    QFile file(filename);
    this->showBase = new ShowBaseClass(this, filename);
    updateTitle();
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

    for(int i = 0; i < showBars->length(); i++)  //Remove all currently loaded ShowBars
    {
        layout()->removeWidget(showBars->at(i));
        showBars->at(i)->hide();
    }
    showBars->clear();
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
        ShowBarWidget *bar = nullptr;
        //TODO add file extension error checking
        int offset = 0;
        QString port = "";
        if(offsets != nullptr)
            offset = offsets->at(i);
        if(ports != nullptr)
            port = ports->at(i);

        QList<QString> currentArgs;
        if(args != nullptr)
            currentArgs = args->at(i);
        if(fileinfo.suffix() == "lsr") {
            if(currentArgs.length() == 0)
                bar = new LEDShowBarWidget(this, pixpersec, file, offset, port, "Green");
            else {
                QString colorName = currentArgs[0];
                bar = new LEDShowBarWidget(this, pixpersec, file, offset, port, colorName );
            }
        } else if(fileinfo.suffix() == "osr") {
            if(currentArgs.length() == 0)
                bar = new MotorShowBarWidget(this, pixpersec, file, offset, port);
            else {
                bool reverse = currentArgs[0] != "0";
                bar = new MotorShowBarWidget(this, pixpersec, file, offset, port, reverse);
            }
        } else {
            bar = new WAVShowBarWidget(this, pixpersec, file, offset, port); //TODO .WAV
        }
        layout()->addWidget(bar);
        showBars->append(bar);
        updateChildren();
        trackShowDataUpdated();
    }
    update();

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

void ShowPrimaryPanel::createEmptyShow()
{
    this->showBase = new ShowBaseClass(this);
}

void ShowPrimaryPanel::save()
{
    showBase->save();
    showChanged = false;
    updateTitle();
}

void ShowPrimaryPanel::saveAs(QFile * newSourceFile)
{
    showBase->saveAs(newSourceFile);
    showChanged = false;
    updateTitle();
}

void ShowPrimaryPanel::trackShowDataUpdated()
{
    if(showBase != nullptr) {
        showChanged = true;
        updateTitle();
    }
}

void ShowPrimaryPanel::removeTrack(ShowBarWidget *track)
{
    layout()->removeWidget(track);
    showBars->removeAll(track);
    track->hide();
    showChanged = true;
}
