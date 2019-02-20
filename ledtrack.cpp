#include "ledtrack.h"
#include <QTextStream>
#include <QRegularExpression>
#include <QPainter>
#include <QPaintEvent>
#include <QDebug>
#include <QFileInfo>
#include <QFileDialog>
#include "leddialog.h"
#include "showprimarypanel.h"
#include "point.h"
#include "portconfig.h"

LEDTrack::LEDTrack(QWidget *parent, float pixpersec, QFile *sourceFile, int offset, QString port) : Track (parent, pixpersec, offset, sourceFile, port)
{
    if(sourceFile == nullptr) {
        qInfo() << "Source File Not Specified";
        return; //TODO create empty track, and pull up properties window.
    }
    if(!sourceFile->open(QIODevice::ReadOnly | QIODevice::Text)) {
        qInfo() << "Source File Not Found";
        return; //TODO throw file cannot be opened window
    }

    QTextStream in(sourceFile);
    QString line;
    QStringList list;

    line = in.readLine();
    list = line.split(QRegularExpression(","));
    if(list[0] != "Title") {
        qInfo() << "Title Not Found";
        return; //TODO File Corrupted
    }
    title = list[1];

    this->port = port;
    setColor();
    /*
    line = in.readLine();
    list = line.split(QRegularExpression(","));
    if(list[0] != "Port") {
        qInfo() << "Port not found!";
        return; //TODO File Corrupted
    }
    port = list[1];

    line = in.readLine();
    list = line.split(QRegularExpression(","));
    if(list[0] != "Color") {
        qInfo() << "Color not found!";
        return; //TODO File Corrupted
    }
    if(list[1] == "Green")
        color = Qt::green;
    else if(list[1] == "Red")
        color = Qt::red;
    else if(list[1] == "Yellow")
        color = Qt::yellow;
    else
        color = Qt::black;
    */

    line = in.readLine();
    if(line != "Points")
        return; //TODO File Corrupted
    points = QList<Point>();
    while((line = in.readLine()) != "EndPoints")
    {
        list = line.split(QRegularExpression(","));
        points.append({list[0].toInt() == 1, list[1].toInt()}); //Value Time
        length = list[1].toInt(); //Will store last time stamp.
    }
    sourceFile->close();

    qInfo() << title;
    qInfo() << port;
    qInfo() << color;
    for(int i = 0; i < points.length(); i++)
        qInfo() << points[i].val << "@" << points[i].ms/1000 << "." << points[i].ms%1000;
    qInfo() << length;
    init();
}

LEDTrack::LEDTrack(QWidget *parent, float pixpersec, QStringList * args, QList<Point> * points)
{
    port = args->at(1);
    title = "";
    this->points = QList<Point>(*points);
    length = points->last().ms;
    init();
}

void LEDTrack::apply(QString name, int offset, QString port)
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

    setColor();

    update();
    if(showChange)
        parentPanel->trackShowDataUpdated();

    if(trackChange)
        this->setTrackChanged(true);
}

void LEDTrack::paintEvent(QPaintEvent *event)
{
    event->accept();
    Track::paintEvent(nullptr);
    QPainter painter(this);
    painter.setPen(color);
    painter.setBrush(color);
    QPointF prevPoint;
    for(int i = 0; i < points.length(); i++) {
        QPointF truePoint(TRACK_HANDLE_WIDTH + TRACK_INFO_WIDTH + augmentedOffset + (pixpersec * points[i].ms / 1000.0), (points[i].val ? .25 : .75) * (this->height()-2) + 1);
        QRectF bounds(truePoint - QPointF(LED_TRACK_POINT_RAD,LED_TRACK_POINT_RAD), truePoint + QPointF(LED_TRACK_POINT_RAD,LED_TRACK_POINT_RAD));
        //qInfo() << bounds;
        painter.drawEllipse(bounds);
        if(i != 0) {
            QPointF intermediate(truePoint.x(), prevPoint.y());
            painter.drawLine(prevPoint, intermediate);
            painter.drawLine(intermediate, truePoint);
        }
        prevPoint = truePoint;
    }
}

void LEDTrack::init() {
    setColor();

    Track::init();
}

void LEDTrack::propertiesOpen()
{
    qInfo() << "Opening LED Properties";
    LEDDialog *dialog = new LEDDialog(this, filename, title, offset, port);
    dialog->show();
}

void LEDTrack::saveTrack()
{
    sourceFile->open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(sourceFile);
    out << "Title," << title << "\n";
    out << "Points\n";
    for(int i = 0; i < points.length(); i++) {
        Point p = points[i];
        out << (p.val ? "1" : "0") << "," << p.ms << "\n";
    }
    out << "EndPoints";
    sourceFile->close();
}

void LEDTrack::saveTrackAs()
{
    QString newFilepath = QFileDialog::getSaveFileName(this, tr("Save Track"),"",tr("Animaniacs LED Files (*.lsr)"));
    if(newFilepath == "")
        newFilepath = "untitled.lsr";
    this->setSourceFile(new QFile(newFilepath));
    saveTrack();
}

void LEDTrack::setColor()
{
    PortConfig::LEDConfig * config = (PortConfig::LEDConfig *) PortConfig::getInstance()->getOutputConfig(port);
    this->colorName = config->color;
    if(colorName == "Green" || colorName == "green" || colorName == "g" || colorName == "G")
        color = Qt::green;
    else if(colorName == "Red" || colorName == "red" || colorName == "r" || colorName == "R")
        color = Qt::red;
    else if(colorName == "Blue" || colorName == "blue" || colorName == "b" || colorName == "B")
        color = Qt::blue;
}

