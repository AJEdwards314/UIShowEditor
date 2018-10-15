#include "showeditorwindow.h"
#include "controlleradapter.h"
#include <QDebug>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ControllerAdapter adapter;
    ShowEditorWindow w(&adapter);
    w.show();

    return a.exec();
}
