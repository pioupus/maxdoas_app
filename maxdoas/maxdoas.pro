# -------------------------------------------------
# Project created by QtCreator 2011-05-18T18:37:50
# -------------------------------------------------
QT += core \
    gui
TARGET = maxdoas
TEMPLATE = app
INCLUDEPATH += ../libs/qserialdevice/src/qserialdevice
INCLUDEPATH += ../libs/qwt-6.0/src
QMAKE_LIBDIR += ../libs/qwt-6.0/lib
QMAKE_LIBDIR += ../libs/qserialdevice/src/build/release
LIBS += -lqserialdevice
LIBS += -lqwt
SOURCES += main.cpp \
    mainwindow.cpp \
    tfrmtempctrl.cpp
HEADERS += mainwindow.h \
    tfrmtempctrl.h
FORMS += mainwindow.ui \
    TfrmTempctrl.ui \
    tfrmtempctrl.ui
