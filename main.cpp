#include "showeditorwindow.h"
#include "controlleradapter.h"
#include "homewindow.h"
#include <QDebug>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ControllerAdapter adapter;
    //adapter.startSerialConnection("COM7", QSerialPort::Baud9600);
    HomeWindow w(&adapter);
    //ShowEditorWindow w(&adapter);
    w.show();

    return a.exec();
}
