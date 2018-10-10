#include "wavtrack.h"
#include <QPainter>
#include <QPaintEvent>
#include <QDebug>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include "wavdialog.h"
#include "showprimarypanel.h"

WAVTrack::WAVTrack(QWidget *parent, float pixpersec, QFile *sourceFile, int offset, QString port)
    : Track (parent, pixpersec, offset, sourceFile, port)
{
    needs_repaint = true;
    if(sourceFile == nullptr) {
        qInfo() << "Source File Not Specified";
        return; //TODO create empty track, and pull up properties window.
    }
    if(!sourceFile->open(QIODevice::ReadOnly)) {
        qInfo() << "Source File Not Found";
        return; //TODO throw file cannot be opened window
    }


    this->title = filename;

    QByteArray blob = sourceFile->readAll();

    sourceFile->close();

    QString format = blob.mid(0,4);
    qInfo() << format;
    if(format != "RIFF") {
        qInfo() << "Not RIFF";
        return;
    }
    format = blob.mid(8, 4);
    qInfo() << format;
    if(format != "WAVE") {
        qInfo() << "Not WAVE";
    }
    int formatStart = blob.indexOf("fmt ");
    qInfo() << formatStart;
    QDataStream *str = new QDataStream(blob.mid(formatStart + 10, 2));
    str->setByteOrder(QDataStream::LittleEndian);
    *str >> chanels;
    delete str;

    str = new QDataStream(blob.mid(formatStart + 12, 4));
    str->setByteOrder(QDataStream::LittleEndian);
    *str >> sampleRate;
    delete str;

    str = new QDataStream(blob.mid(formatStart + 22, 2));
    str->setByteOrder(QDataStream::LittleEndian);
    *str >> bytesPerSample;
    delete str;

    qInfo() << "Chanels: " << chanels;
    qInfo() << "Sample Rate: " << sampleRate;
    qInfo() << "Bytes / Sample: " << bytesPerSample;
    bytesPerSample /= 8;
    int dataStart = blob.indexOf("data");

    str = new QDataStream(blob.mid(dataStart + 4, 4));
    str->setByteOrder(QDataStream::LittleEndian);
    qint32 rawLength;
    *str >> rawLength;
    delete str;



    qInfo() << rawLength;
    length = (rawLength / chanels / bytesPerSample) * 1.0 / sampleRate * 1000;

    qint16 sampleIn;
    qint16 sum = 0;
    str = new QDataStream(blob.mid(dataStart + 8));
    str->setByteOrder(QDataStream::LittleEndian);
    qInfo() << dataStart;
    for(int i = dataStart + 8; i < blob.length(); i += chanels * bytesPerSample) {
        sum = 0;
        for(int j = 0; j < chanels; j++) {
            *str >> sampleIn;
            sum += sampleIn;
        }
        samples.append(sum / chanels);
        absMax = qMax(qAbs(sum / chanels), absMax);
    }
    delete str;

    qInfo() << chanels;
    qInfo() << bytesPerSample;
    qInfo() << sampleRate;
    qInfo() << length;
    updateSize();
}

void WAVTrack::apply(int offset, QString port)
{
    bool showChange = false;

    if(this->offset != offset)
        showChange = true;
    setOffset(offset);

    if(this->port != port)
        showChange = true;
    this->port = port;

    update();
    parentPanel->trackShowDataUpdated();
}

void WAVTrack::paintEvent(QPaintEvent *event)
{
    event->accept();
    Track::paintEvent(event);
    QPainter painter(this);
    QRect size(TRACK_HANDLE_WIDTH + TRACK_INFO_WIDTH + augmentedOffset,0, length * pixpersec / 1000, this->height());
    painter.setPen(Qt::black);
    painter.setBrush(Qt::black);
    painter.drawPixmap(size, pixmap, pixmap.rect());
    qInfo() << pixmap.rect();
    if(needs_repaint) {
        repaintWAV();
        needs_repaint = false; // Do this once.  Time consuming.  Unless track updated.
    }
}

void WAVTrack::propertiesOpen()
{
    qInfo() << "Opening WAV Properties";
    WAVDialog *dialog = new WAVDialog(this, filename, offset, port);
    dialog->show();
}

void WAVTrack::saveTrack()
{
    QMessageBox msgBox;
    msgBox.setText("No Point Saving a WAV File.");
    msgBox.setInformativeText("Would you like to Save As instead??");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    switch(ret) {
    case QMessageBox::Ok:
        saveTrackAs();
        break;
    }
}

void WAVTrack::saveTrackAs()
{
    QString newFilepath = QFileDialog::getSaveFileName(this, tr("Save Track"),"",tr("WAV Files (*.wav)"));
    qInfo() << "Dialog Returned";
    if(newFilepath == "") {
        return;
    }
    QString currentFilepath = QFileInfo(*sourceFile).absoluteFilePath();
    QFile::remove(newFilepath);
    QFile::copy(currentFilepath, newFilepath);
    this->setSourceFile(new QFile(newFilepath));
}

void WAVTrack::repaintWAV()
{
    pixmap = QPixmap(samples.length() / WAV_SHOW_PNT_DENSITY, this->height());
    pixmap.fill(Qt::transparent);
    //pixmap.fill(Qt::blue);
    QPainter painter(&pixmap);
    painter.setPen(Qt::black);
    painter.setBrush(Qt::blue);
    //painter.fillRect(pixmap.rect(),painter.brush());
    QPointF prevPoint;
    for(int i = 0; i < samples.length(); i += WAV_SHOW_PNT_DENSITY) {
        QPointF truePoint(i / WAV_SHOW_PNT_DENSITY, (1.0 * samples[i] / (absMax) + .5) * (this->height()-2) + 1);
        //qInfo() << truePoint;
        if(i != 0) {
            painter.drawLine(prevPoint, truePoint);
        }
        prevPoint = truePoint;
    }
    //painter.drawRect(10, 10, 100, 100);
    //qInfo() << pixmap->save("ISDF.bmp");
    //pixmap->load("C:\\Users\\alex\\OneDrive\\Documents\\Systems\\UI Test\\Test Files\\Pic.jpg");
    qInfo() << pixmap.save("C:\\Users\\alex\\OneDrive\\Documents\\Systems\\UI Test\\Test Files\\ISDF.bmp");
    qInfo() << "saved";
    qInfo() << samples.length() / WAV_SHOW_PNT_DENSITY;
}


