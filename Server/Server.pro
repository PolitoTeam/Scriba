#-------------------------------------------------
#
# Project created by QtCreator 2019-09-05T10:21:39
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Server
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

CONFIG += c++11

SOURCES += \
        database.cpp \
        main.cpp \
        mongo.cpp \
        server.cpp \
        serverwindow.cpp \
        serverworker.cpp

HEADERS += \
        database.h \
        mongo.h \
        serializeSize.h \
        server.h \
        serverwindow.h \
        serverworker.h

FORMS += \
        serverwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# library for password hashing

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../3rdparty/libsodium/1.0.18/lib/release/ -lsodium
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../3rdparty/libsodium/1.0.18/lib/debug/ -lsodium
else:macx: LIBS += -L$$PWD/../3rdparty/libsodium/1.0.18/lib/macOS -lsodium
else:unix: LIBS += -L$$PWD/../3rdparty/libsodium/1.0.18/lib/linux -lsodium

unix:CONFIG(debug, debug|release): LIBS += -L/usr/local/lib -lmongocxx -lbsoncxx
#unix:CONFIG(debug, debug|release): LIBS += -lmongocxx -lbsoncxx

#c++ --std=c++11 test.cpp -o test -I/usr/local/include/mongocxx/v_noabi -I/usr/local/include/bsoncxx/v_noabi -L /usr/local/lib -lmongocxx -lbsoncxx
#/home/enrico/mongo-cxx-driver

INCLUDEPATH += /usr/local/include/mongocxx/v_noabi
INCLUDEPATH += /usr/local/include/bsoncxx/v_noabi

INCLUDEPATH += $$PWD/../3rdparty/libsodium/1.0.18/include
DEPENDPATH += $$PWD/../3rdparty/libsodium/1.0.18/include

INCLUDEPATH += $$PWD/../3rdparty

RESOURCES += \
    server.qrc

LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
