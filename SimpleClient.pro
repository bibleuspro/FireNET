QT       += core gui
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SimpleClient
TEMPLATE = app

MOC_DIR += $$PWD/temp/moc/client
OBJECTS_DIR += $$PWD/temp/obj/client

SOURCES += src/client/main.cpp\
        src/client/mainwindow.cpp

HEADERS  += src/client/mainwindow.h

FORMS    += src/client/mainwindow.ui
