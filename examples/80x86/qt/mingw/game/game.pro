#-------------------------------------------------
#
# Project created by QtCreator 2012-06-11T14:58:33
#
#-------------------------------------------------

TEMPLATE = app
QT      += core gui
DEFINES += QT_NO_STATEMACHINE

TARGET   = game

INCLUDEPATH = . \
    $(QPN)/ports/qt \
    $(QPN)/include

HEADERS  += \
    $(QPN)/ports/qt/qpn_app.h \
    qpn_port.h \
    gui.h \
    bsp.h \
    game.h

SOURCES +=\
    $(QPN)/source/qepn.c \
    $(QPN)/ports/qt/qpn_app.cpp \
    missile.c \
    mine2.c \
    mine1.c \
    gui.cpp \
    tunnel.c \
    main.cpp \
    bsp.cpp \
    ship.c

RESOURCES = gui.qrc

#FORMS += mainwindow.ui
