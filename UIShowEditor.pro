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
    showbarwidget.cpp \
    showprimarypanel.cpp \
    motorshowbarwidget.cpp \
    ledshowbarwidget.cpp \
    showbaseclass.cpp \
    wavshowbarwidget.cpp \
    leddialog.cpp \
    motordialog.cpp \
    wavdialog.cpp

HEADERS += \
        showeditorwindow.h \
    showbarwidget.h \
    showprimarypanel.h \
    motorshowbarwidget.h \
    ledshowbarwidget.h \
    showbaseclass.h \
    wavshowbarwidget.h \
    ledproperties.h \
    leddialog.h \
    motordialog.h \
    wavdialog.h

FORMS += \
        showeditorwindow.ui \
    leddialog.ui \
    motordialog.ui \
    wavdialog.ui

RC_ICONS = \
        team_logo.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
