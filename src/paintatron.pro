#-------------------------------------------------
#
# Project created by QtCreator 2013-03-15T11:07:21
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = paintatron
TEMPLATE = app

INCLUDEPATH += /usr/include
LIBS += -L/usr/lib -lgpr -lm -lz -fopenmp

SOURCES += main.cpp\
        mainwindow.cpp \
    paintatron.cpp \
    dialogsaveimage.cpp \
    dialogabout.cpp

HEADERS  += mainwindow.h \
    paintatron.h \
    dialogsaveimage.h \
    dialogabout.h

FORMS    += mainwindow.ui \
    dialogsaveimage.ui \
    dialogabout.ui
