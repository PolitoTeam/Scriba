#-------------------------------------------------
#
# Project created by QtCreator 2019-07-13T20:49:54
#
#-------------------------------------------------

QT       += core gui sql network

qtHaveModule(printsupport): QT += printsupport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Client
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


SOURCES += \
    CRDT.cpp \
    appMainWindow.cpp \
    client.cpp \
    highlighter.cpp \
    login.cpp \
    editor.cpp \
    main.cpp \
    home.cpp \
    modify.cpp \
    signup.cpp

HEADERS += \
    CRDT.h \
    appMainWindow.h \
    client.h \
    highlighter.h \
    login.h \
    editor.h \
    home.h \
    modify.h \
    signup.h \
    symbol.h

FORMS += \
    index.ui \
    login.ui \
    editor.ui \
    home.ui \
    modify.ui \
    signup.ui

RESOURCES += \
    client.qrc
