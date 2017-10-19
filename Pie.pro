#-------------------------------------------------
#
# Project created by QtCreator 2015-07-21T09:09:09
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Pie
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    PieWidget.cpp \
    FileSystem.cpp \
    DirInfo.cpp \
    SliceWidget.cpp \
    GoogleColors.cpp

HEADERS  += MainWindow.h \
    PieWidget.h \
    FileSystem.h \
    DirInfo.h \
    SliceWidget.h \
    GoogleColors.h

FORMS    += MainWindow.ui

CONFIG += c++11
