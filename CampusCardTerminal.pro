#-------------------------------------------------
#
# Project created by QtCreator 2015-12-09T16:56:23
#
#-------------------------------------------------

QT       += core gui
QT       += serialport
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CampusCardTerminal
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qextserial/qextserialport.cpp \
    qextserial/qextserialport_win.cpp \
    readercmd.cpp \
    aboutdialog.cpp \
    authenticatedialog.cpp

HEADERS  += mainwindow.h \
    qextserial/qextserialport.h \
    qextserial/qextserialport_global.h \
    qextserial/qextserialport_p.h \
    readercmd.h \
    aboutdialog.h \
    authenticatedialog.h

FORMS    += mainwindow.ui \
    aboutdialog.ui \
    authenticatedialog.ui

RESOURCES += \
    resource.qrc
