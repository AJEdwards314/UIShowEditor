#include "motortrack.h"

#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>
#include <QPainter>
#include <QFile>
#include <QPaintEvent>
#include <QFileInfo>
#include <QFileDialog>
#include "motordialog.h"
#include "showprimarypanel.h"
#include "point.h"
#include "portconfig.h"

MotorTrack::MotorTrack(QWidget *parent, float pixpersec, QFile *sourceFile, int offset, QString port, bool reverse) : Track (parent, pixpersec, offset, sourceFile, port)
{
    if(sourceFile == nullptr) {
        qInfo("Source File Not Specified");
        return; //TODO create empty track, and pull up properties window.
    }
    if(!sourceFile->open(QIODevice::ReadOnly | QIODevice::Text)) {
        qInfo("Source File Not Found");
        return; //TODO throw file cannot be opened window
    }

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

    this->reverse = reverse;
    /*
    line = in.readLine();
    list = line.split(QRegularExpression(","));
    if(list[0] != "Port")
        return; //TODO File Corrupted
    port = list[1];

    line = in.readLine();
    list = line.split(QRegularExpression(","));
    if(list[0] != "Reverse")
        return; //TODO File Corrupted
    reverse =  list[1].toInt();
    */

    line = in.readLine();
    list = line.split(QRegularExpression(","));
    while(list[0] != "Points") {
        if(list[0] == "MaxVal") {
            //maxVal =  list[1].toInt();
        } else if(list[0] == "MinVal") {
            //minVal = list[1].toInt();
        } else if(list[0] == "MidVal") {
            //midVal = list[1].toInt();
        }

        line = in.readLine();
        list = line.split(QRegularExpression(","));
    }

    if(line != "Points")
        return; //TODO File Corrupted
    points = QList<Point>();
    while((line = in.readLine()) != "EndPoints")
    {
        list = line.split(QRegularExpression(","));
        points.append({list[0].toInt(), list[1].toInt()}); //Value Time
        length = list[1].toInt(); //Will store last time stamp.
    }
    sourceFile->close();

    //qInfo() << title;
    //qInfo() << port;
    //qInfo() << maxVal;
    //qInfo() << minVal;
    //qInfo() << midVal;
    //for(int i = 0; i < points.length(); i++)
    //    qInfo() << points[i].val << "@" << points[i].ms/1000 << "." << points[i].ms%1000;
    //qInfo() << length;
    init();
}

MotorTrack::MotorTrack(QWidget *parent, float pixpersec, QStringList * args, QList<Point> * points) : Track(parent, pixpersec)
{
    port = args->at(1);
    title = "untitled";
    this->points = QList<Point>(*points); //Copy points over
    length = points->last().ms;
    init();
}


void MotorTrack::apply(QString name, int offset, QString port)
{
    bool showChange = false;
    bool trackChange = false;

    if(this->title != name)
        trackChange = true;
    this->title = name;

    if(this->offset != offset)
        showChange = true;
    setOffset(offset);

    if(this->port != port)
        showChange = true;
    this->port = port;

    update();
    if(showChange)
        parentPanel->trackShowDataUpdated();

    if(trackChange)
        this->setTrackChanged(true);
}

void MotorTrack::paintEvent(QPaintEvent *event)
{
    event->accept();
    Track::paintEvent(nullptr);
    QPainter painter(this);
    painter.setBrush(Qt::black);
    painter.setPen(QPen(Qt::black, 0, Qt::DashLine));
    painter.drawLine(TRACK_HANDLE_WIDTH + TRACK_INFO_WIDTH + augmentedOffset,
                     ((float)((maxVal + 20) - midVal))/((maxVal + 20) - (minVal - 20)) * (this->height()-2) + 1,
                     TRACK_HANDLE_WIDTH + TRACK_INFO_WIDTH + augmentedOffset + augmentedLength,
                     ((float)((maxVal + 20) - midVal))/((maxVal + 20) - (minVal - 20)) * (this->height()-2) + 1);
    painter.setPen(QPen(Qt::black));
    QPointF prevPoint;
    for(int i = 0; i < points.length(); i++) {
        QPointF truePoint(TRACK_HANDLE_WIDTH + TRACK_INFO_WIDTH + augmentedOffset + (pixpersec * points[i].ms / 1000.0), ((float)((maxVal + 20) - points[i].val))/((maxVal + 20) - (minVal - 20)) * (this->height()-2) + 1);
        QRectF bounds(truePoint - QPointF(MOTOR_TRACK_POINT_RAD,MOTOR_TRACK_POINT_RAD), truePoint + QPointF(MOTOR_TRACK_POINT_RAD,MOTOR_TRACK_POINT_RAD));
        //qInfo() << bounds;
        painter.drawEllipse(bounds);
        if(i != 0)
            painter.drawLine(prevPoint, truePoint);
        prevPoint = truePoint;
    }
}

void MotorTrack::init()
{
    setLimits();

    Track::init();
}

void MotorTrack::propertiesOpen()
{
    qInfo() << "Opening Motor Properties";
    MotorDialog *dialog = new MotorDialog(this, filename, title, offset, port);
    dialog->show();
}

void MotorTrack::saveTrack()
{
    sourceFile->open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(sourceFile);
    out << "Title," << title << "\n";
    //out << "MaxVal," << maxVal << "\n";
    //out << "MinVal," << minVal << "\n";
    //out << "MidVal," << midVal << "\n";
    out << "Points\n";
    for(int i = 0; i < points.length(); i++) {
        Point p = points[i];
        out << p.val << "," << p.ms << "\n";
    }
    out << "EndPoints";
    sourceFile->close();
}

void MotorTrack::saveTrackAs()
{
    QString newFilepath = QFileDialog::getSaveFileName(this, tr("Save Track"),"",tr("Animaniacs Motor Files (*.osr)"));
    if(newFilepath == "")
        newFilepath = "untitled.osr";
    this->setSourceFile(new QFile(newFilepath));
    saveTrack();
}

void MotorTrack::setLimits() {
    PortConfig::SRVConfig * config = (PortConfig::SRVConfig *) PortConfig::getInstance()->getOutputConfig(port);

    minVal = config->minVal;
    midVal = config->midVal;
    maxVal = config->maxVal;
    reverse = config->reverse;

    update();
}
