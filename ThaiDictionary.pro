#-------------------------------------------------
#
# Project created by QtCreator 2013-11-01T16:59:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ThaiDictionary
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    sortfilterproxymodel.cpp \
    lineedit.cpp

HEADERS  += mainwindow.h \
    sortfilterproxymodel.h \
    lineedit.h

FORMS    += mainwindow.ui

RESOURCES += \
    dic.qrc
