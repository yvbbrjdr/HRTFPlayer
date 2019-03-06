QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HRTFPlayer
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++11

SOURCES += \
    main.cpp \
    widget.cpp \
    utils.cpp \
    sofa.cpp \
    player.cpp \
    topview.cpp

HEADERS += \
    widget.h \
    utils.h \
    sofa.h \
    player.h \
    topview.h

FORMS += \
    widget.ui

LIBS += \
    -lmysofa \
    -lbass \
    -lbassenc

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
