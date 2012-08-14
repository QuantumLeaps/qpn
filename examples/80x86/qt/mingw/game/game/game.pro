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
    $(QPC)/include \
    $(QPC)/ports/80x86/qt_1t/mingw \

SOURCES +=\
    missile.c \
    mine2.c \
    mine1.c \
    gui.cpp \
    tunnel.c \
    main.cpp \
    bsp.cpp \
    ship.c

HEADERS  += \
    game.h \
    bsp.h \
    gui.h


CONFIG(debug, debug|release) {
    DEFINES += Q_SPY
    LIBS += -L$(QPC)/ports/80x86/qt_1t/mingw/debug

} else {
    DEFINES += NDEBUG
    LIBS += -L$(QPC)/ports/80x86/qt_1t/mingw/release
}

LIBS += -lqp

RESOURCES = gui.qrc

#FORMS += mainwindow.ui
