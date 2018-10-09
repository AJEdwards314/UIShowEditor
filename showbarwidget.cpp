#include "showbarwidget.h"

#include <QPainter>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QPixmap>
#include <QDebug>
#include <QString>
#include <QStaticText>
#include <QMenu>
#include <QFileInfo>
#include "showprimarypanel.h"

ShowBarWidget::ShowBarWidget(QWidget *parent, float pixpersec, int offset, QFile *sourceFile, QString port) : QWidget(parent)
{
    this->index = 0;
    this->offset = offset;
    this->port = port;
    this->title = "notitle";
    this->length = 0;
    this->pixpersec = pixpersec;
    this->setSourceFile(sourceFile);
    parentPanel = (ShowPrimaryPanel*)parent;
    updateSize();
}

void ShowBarWidget::setScale(float pixpersec, int width)
{
    this->pixpersec = pixpersec;
    this->idealWidth = width;
    updateSize();
}

void ShowBarWidget::updateSize()
{
    augmentedLength = ((pixpersec * length) / 1000.0);
    augmentedOffset = ((pixpersec * offset) / 1000.0);
    QSize maxSizePoint(idealWidth, SHOW_BAR_HEIGHT);
    QSize minSizePoint(idealWidth, SHOW_BAR_HEIGHT);
    this->setMaximumSize(maxSizePoint);
    this->setMinimumSize(minSizePoint);
    handle = QRectF(0, 0, SHOW_BAR_HANDLE_WIDTH, height() - 1);
    infoBox = QRectF(SHOW_BAR_HANDLE_WIDTH, 0, SHOW_BAR_INFO_WIDTH, height() - 1);
}

void ShowBarWidget::paintEvent(QPaintEvent *)
{

    QPainter painter(this);
    QRectF size = QRectF(0, 0, this->width() - 1, this->height() - 1);
    QRectF activeRect = QRectF(SHOW_BAR_HANDLE_WIDTH + SHOW_BAR_INFO_WIDTH + augmentedOffset - 5, 1, augmentedLength + 10, this->height() - 3);
    painter.setBrush(Qt::transparent);
    painter.drawRect(size);
    painter.setBrush(QColor(255, 240, 210));
    painter.drawRect(SHOW_BAR_HANDLE_WIDTH, 0, SHOW_BAR_INFO_WIDTH - 5, height() - 1);
    painter.setBrush(QColor(200, 200, 200));
    painter.drawRect(activeRect);
    //painter.setBrush(Qt::red);
    //painter.drawPie(activeRect, 0, index*160);
    drawHandle(painter);
    QStaticText filenameText(filename + (trackChanged ? "*" : ""));
    painter.drawStaticText(SHOW_BAR_HANDLE_WIDTH + 3,0,filenameText);
    QStaticText titleText(title);
    painter.drawStaticText(SHOW_BAR_HANDLE_WIDTH + 3,13,titleText);
    QStaticText lengthText("Length: " + QString::number(length / 1000.0, 'f', 3) + "s");
    painter.drawStaticText(SHOW_BAR_HANDLE_WIDTH + 3,26,lengthText);
    QStaticText offsetText("Offset: " + QString::number(offset / 1000.0, 'f', 3) + "s");
    painter.drawStaticText(SHOW_BAR_HANDLE_WIDTH + 3,39,offsetText);
}

void ShowBarWidget::drawHandle(QPainter &painter) {
    painter.setBrush(Qt::darkGray);
    painter.drawRect(handle);
}

void ShowBarWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        if(handle.contains(event->pos()))
            handleClicked(event);
    } else if(event->button() == Qt::RightButton) {
        if(infoBox.contains(event->pos()))
            infoRightClicked(event);
    }

}

void ShowBarWidget::handleClicked(QMouseEvent *event)
{
    ((QVBoxLayout *)this->parentWidget()->layout())->removeWidget(this);
    ((ShowPrimaryPanel*)parentWidget())->objectGrabbed(this);

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    mimeData->setText("");
    drag->setMimeData(mimeData);
    drag->setPixmap(grab());
    setVisible(false);

    Qt::DropAction dropAction = drag->exec();
}

void ShowBarWidget::infoRightClicked(QMouseEvent *event)
{
    qInfo() << "Info Right Clicked!!!";
    QMenu *menu = new QMenu(this);

    QAction *saveTrackAction = new QAction(tr("Save Track"), this);
    saveTrackAction->setStatusTip(tr("Save this Track"));
    connect(saveTrackAction, &QAction::triggered, this, &ShowBarWidget::saveTrackParent);
    menu->addAction(saveTrackAction);

    QAction *saveTrackAsAction = new QAction(tr("Save Track As"), this);
    saveTrackAsAction->setStatusTip(tr("Save this Track Under New Name"));
    connect(saveTrackAsAction, &QAction::triggered, this, &ShowBarWidget::saveTrackAsParent);
    menu->addAction(saveTrackAsAction);

    menu->addSeparator();

    QAction *removeTrackAction = new QAction(tr("Remove Track"), this);
    saveTrackAsAction->setStatusTip(tr("Remove this Track from the Show"));
    connect(removeTrackAction, &QAction::triggered, this, &ShowBarWidget::removeTrack);
    menu->addAction(removeTrackAction);

    menu->addSeparator();

    QAction *propertiesAction = new QAction(tr("Properties"), this);
    propertiesAction->setStatusTip(tr("Edit Properties of this Track"));
    connect(propertiesAction, &QAction::triggered, this, &ShowBarWidget::propertiesOpen);
    menu->addAction(propertiesAction);
    menu->popup(event->globalPos());
}

int ShowBarWidget::getBaseWidth()
{
    return (int)(SHOW_BAR_HANDLE_WIDTH + SHOW_BAR_INFO_WIDTH + augmentedOffset + augmentedLength + 7);

}

void ShowBarWidget::propertiesOpen()
{
    qInfo() << "Opening Properties";
}

void ShowBarWidget::setOffset(int offset)
{
    this->offset = offset;
    updateSize();
}

void ShowBarWidget::saveTrackParent()
{
    if(this->sourceFile == nullptr) {
        saveTrackAsParent();
        return;
    }
    saveTrack();
    setTrackChanged(false);
}

void ShowBarWidget::saveTrackAsParent()
{
    saveTrackAs();
    setTrackChanged(false);
    parentPanel->trackShowDataUpdated();
}

void ShowBarWidget::setTrackChanged(bool trackChanged)
{
    this->trackChanged = trackChanged;
    update();
}

void ShowBarWidget::setSourceFile(QFile * sourceFile)
{
    this->sourceFile = sourceFile;
    this->filename = QFileInfo(*sourceFile).fileName();
}

void ShowBarWidget::removeTrack()
{
    qInfo() << "removing track";
    parentPanel->removeTrack(this);
}
