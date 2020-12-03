#-------------------------------------------------
#
# Project created by QtCreator 2019-07-19T10:52:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET_VERSION = 0.2.0
TARGET = camapp-dahua.$$TARGET_VERSION
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

QMAKE_CXXFLAGS += -std=c++0x
CONFIG += c++11
SOURCES += \
        main.cpp \
        mainwindow.cpp \
    capture.cpp \
    parser.cpp \
    cameraselectdialog.cpp \
    Record.cpp

HEADERS += \
        mainwindow.h \
    capture.h \
    parser.h \
    cameraselectdialog.h \
    cameraModel.h \
    Record.h

FORMS += \
        mainwindow.ui \
    cameraselectdialog.ui

INCLUDEPATH += \
    /usr/include/gstreamer-1.0 \
    /usr/include/gstreamermm-1.0/ \
    /usr/include/glib-2.0 \
    /usr/include/jsoncpp \
    /usr/lib/i386-linux-gnu/glib-2.0/include
unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += gstreamer-1.0 gstreamer-app-1.0 gstreamer-plugins-base-1.0 opencv jsoncpp

DISTFILES += \
    README.md

RESOURCES += \
    toolbar.qrc
