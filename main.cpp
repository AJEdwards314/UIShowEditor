#include "showeditorwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ShowEditorWindow w;
    w.show();
    //w2.show();

    return a.exec();
}
