#-------------------------------------------------
#
# Project created by QtCreator 2018-09-17T15:31:00
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = UIShowEditor
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        showeditorwindow.cpp \
    showprimarypanel.cpp \
    showbaseclass.cpp \
    leddialog.cpp \
    motordialog.cpp \
    wavdialog.cpp \
    ledtrack.cpp \
    motortrack.cpp \
    wavtrack.cpp \
    track.cpp \
    controlleradapter.cpp \
    serialsettingsdialog.cpp \
    recordingconfigurationdialog.cpp \
    behaviorprogramming.cpp \
    homewindow.cpp \
    portconsts.cpp \
    portconfig.cpp \
    portconfigdialog.cpp \
    outputportconfigwidget.cpp \
    inputportconfigwidget.cpp \
    porttestdialog.cpp \
    portreaddaemon.cpp \
    serialtxdaemon.cpp \
    serialrxdaemon.cpp \
    workingdirectory.cpp

HEADERS += \
        showeditorwindow.h \
    showprimarypanel.h \
    showbaseclass.h \
    leddialog.h \
    motordialog.h \
    wavdialog.h \
    ledtrack.h \
    motortrack.h \
    track.h \
    wavtrack.h \
    controlleradapter.h \
    point.h \
    serialsettingsdialog.h \
    recordingconfigurationdialog.h \
    behaviorprogramming.h \
    homewindow.h \
    portconsts.h \
    portconfig.h \
    portconfigdialog.h \
    outputportconfigwidget.h \
    inputportconfigwidget.h \
    porttestdialog.h \
    portreaddaemon.h \
    serialtxdaemon.h \
    serialrxdaemon.h \
    workingdirectory.h


FORMS += \
        showeditorwindow.ui \
    leddialog.ui \
    motordialog.ui \
    wavdialog.ui \
    serialsettingsdialog.ui \
    recordingconfiguration.ui \
    behaviorprogramming.ui \
    homewindow.ui \
    portconfigdialog.ui \
    outputportconfigwidget.ui \
    inputportconfigwidget.ui \
    porttestdialog.ui

QT += \
    serialport

RC_ICONS = \
        team_logo.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    resources.qrc
