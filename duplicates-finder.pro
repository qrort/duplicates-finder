
#-------------------------------------------------
#
# Project created by QtCreator 2018-12-22T16:52:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = duplicates-finder
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS_RELEASE += -O3

SOURCES += Sources/main.cpp\
           Sources/mainwindow.cpp\
           Sources/hasher.cpp \
           Sources/askwidget.cpp


HEADERS  += Headers/mainwindow.h\
            Headers/hasher.h \
            Headers/askwidget.h


FORMS    += Forms/mainwindow.ui \
            Forms/askwidget.ui
