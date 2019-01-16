#-------------------------------------------------
#
# Project created by QtCreator 2018-12-22T16:52:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = duplicates-finder
TEMPLATE = app


SOURCES += main.cpp\
           mainwindow.cpp\
           hasher.cpp \
    askwidget.cpp



HEADERS  += mainwindow.h\
            hasher.h \
    askwidget.h


FORMS    += mainwindow.ui \
    askwidget.ui
