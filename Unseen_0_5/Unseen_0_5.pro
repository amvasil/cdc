#-------------------------------------------------
#
# Project created by QtCreator 2014-10-04T12:10:36
#
#-------------------------------------------------

QT       += core gui
QT += network
CONFIG += qwt
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Unseen_0_5
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    general_classes/configuration.cpp \
    general_classes/generator.cpp \
    general_classes/oscilloscope.cpp \
    general_classes/pitaya.cpp \
    general_classes/project.cpp \
    general_classes/supply.cpp \
    general_classes/test.cpp \
    general_classes/variable.cpp \
    net/netclient.cpp \
    net/netserver.cpp

HEADERS  += mainwindow.h \
    general_classes/configuration.h \
    general_classes/generator.h \
    general_classes/oscilloscope.h \
    general_classes/pitaya.h \
    general_classes/project.h \
    general_classes/supply.h \
    general_classes/test.h \
    general_classes/variable.h \
    net/netclient.h \
    net/netserver.h

FORMS    += mainwindow.ui
