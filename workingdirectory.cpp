#include "workingdirectory.h"

QDir WorkingDirectory::dir;

WorkingDirectory::WorkingDirectory()
{}

void WorkingDirectory::setDir(QString dirString) {
    dir = QDir(dirString);
}

QDir WorkingDirectory::getDir() {
    return dir;
}

QString WorkingDirectory::getPath() {
    return dir.absolutePath();
}
