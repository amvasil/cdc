#-------------------------------------------------
#
# Project created by QtCreator 2014-10-04T21:02:37
#
#-------------------------------------------------

QT       += core
QT += network
QT       -= gui

TARGET = PitayaServer_0_2
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    general_classes/configuration.cpp \
    general_classes/generator.cpp \
    general_classes/oscilloscope.cpp \
    general_classes/pitaya.cpp \
    general_classes/project.cpp \
    general_classes/supply.cpp \
    general_classes/test.cpp \
    general_classes/variable.cpp \
    net/netclient.cpp \
    net/netserver.cpp \
    generatormanager.cpp \
    oscilloscopemanager.cpp \
    pitayamainserver.cpp \
    hardware/fpga_awg.c \
    hardware/fpga_osc.c \
    hardware/main_osc.c \
    hardware/worker.c

HEADERS += \
    general_classes/configuration.h \
    general_classes/generator.h \
    general_classes/oscilloscope.h \
    general_classes/pitaya.h \
    general_classes/project.h \
    general_classes/supply.h \
    general_classes/test.h \
    general_classes/variable.h \
    net/netclient.h \
    net/netserver.h \
    generatormanager.h \
    oscilloscopemanager.h \
    pitayamainserver.h \
    hardware/fpga_awg.h \
    hardware/fpga_osc.h \
    hardware/main_osc.h \
    hardware/worker.h
