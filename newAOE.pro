#-------------------------------------------------
#
# Project created by QtCreator 2023-03-04T16:37:38
#
#-------------------------------------------------

QT       += \
        core gui \
        multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = newAOE
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
    AI_My.cpp \
        main.cpp \
        MainWidget.cpp \
    GameWidget.cpp \
    Coordinate.cpp \
    Block.cpp \
    Map.cpp \
    Building.cpp \
    Player.cpp \
    Ruin.cpp \
    Coores.cpp \
    MoveObject.cpp \
    Animal.cpp \
    Human.cpp \
    Farmer.cpp \
    GlobalVariate.cpp \
    ActWidget.cpp \
    SelectWidget.cpp \
    ViewWidget.cpp \
    Missile.cpp \
    AI.cpp \
    AboutDialog.cpp \
    Option.cpp

HEADERS += \
    AI_My.h \
        MainWidget.h \
    GameWidget.h \
    Coordinate.h \
    Block.h \
    config.h \
    Map.h \
    Building.h \
    Player.h \
    Ruin.h \
    Coores.h \
    GlobalVariate.h \
    MoveObject.h \
    Animal.h \
    Human.h \
    Farmer.h \
    ActWidget.h \
    SelectWidget.h \
    ViewWidget.h \
    Missile.h \
    AI.h \
    AboutDialog.h \
    Option.h

FORMS += \
        MainWidget.ui \
    GameWidget.ui \
    SelectWidget.ui \
    AboutDialog.ui \
    Option.ui

DISTFILES += \
    res.rcc

