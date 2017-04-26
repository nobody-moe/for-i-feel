#-------------------------------------------------
#
# Project created by QtCreator 2017-04-26T12:24:19
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia

TARGET = card_game
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    board.cpp \
    card.cpp \
    carddrawer.cpp \
    cardswidget.cpp \
    deck.cpp \
    util.cpp

HEADERS  += mainwindow.h \
    board.h \
    card.h \
    carddrawer.h \
    cardswidget.h \
    Constants.h \
    deck.h \
    util.h

FORMS    +=

RESOURCES += \
    puzzle.qrc

DISTFILES +=
