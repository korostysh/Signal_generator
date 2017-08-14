#-------------------------------------------------
#
# Project created by QtCreator 2016-11-11T23:29:44
#
#-------------------------------------------------

QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = labwork
TEMPLATE = app
CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    inputvalidatortableitem.cpp \
    qcustomplot-source/qcustomplot.cpp \
    plottingwindow.cpp

HEADERS  += mainwindow.h \
    inputvalidatortableitem.h \
    qcustomplot-source/qcustomplot.h \
    plottingwindow.h \
    rfsc.h

FORMS    += mainwindow.ui \
    plottingwindow.ui

RESOURCES += \
    res.qrc


