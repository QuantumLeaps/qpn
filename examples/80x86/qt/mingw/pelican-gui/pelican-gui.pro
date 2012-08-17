#-------------------------------------------------
#
# Project created by QtCreator 2012-05-10T10:48:40
#
#-------------------------------------------------
TEMPLATE = app

QT      += core gui
TARGET   = pelican-gui
DEFINES += QT_NO_STATEMACHINE

INCLUDEPATH += . \
    $(QPN)/ports/qt \
    $(QPN)/include

HEADERS += \
    $(QPN)/ports/qt/qpn_app.h \
    qpn_port.h \
    gui.h \
    bsp.h \
    pelican.h

SOURCES += \
    $(QPN)/source/qepn.c \
    $(QPN)/ports/qt/qpn_app.cpp \
    main.cpp \
    gui.cpp \
    bsp.cpp \
    pelican.c

FORMS += gui.ui

RESOURCES = gui.qrc

RESOURCES = gui.qrc

win32:RC_FILE = gui.rc
