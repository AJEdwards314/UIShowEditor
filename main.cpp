#include "showeditorwindow.h"
#include "controlleradapter.h"
#include "homewindow.h"
#include <QDebug>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ControllerAdapter::setInstance();
    //adapter.startSerialConnection("COM7", QSerialPort::Baud9600);
    HomeWindow w;
    //ShowEditorWindow w(&adapter);
    w.show();

    return a.exec();
}
