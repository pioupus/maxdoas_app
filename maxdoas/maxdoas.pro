# -------------------------------------------------
# Project created by QtCreator 2011-05-18T18:37:50
# -------------------------------------------------
QT += core \
    gui
TARGET = maxdoas
TEMPLATE = app
INCLUDEPATH += ../libs/qserialdevice/src/qserialdevice
INCLUDEPATH += ../libs/qwt-6.0/src
INCLUDEPATH += ../libs/qwt-6.0/src
INCLUDEPATH += /usr/lib/jvm/default-java/include
INCLUDEPATH += /usr/lib/jvm/default-java/include/linux
INCLUDEPATH += /opt/OceanOptics/OmniDriverSPAM/include
QMAKE_LIBDIR += ../libs/qwt-6.0/lib
QMAKE_LIBDIR += ../libs/qserialdevice/src/build/release

# QMAKE_LIBDIR += /opt/OceanOptics/OmniDriverSPAM/_jvm/lib/i386/server
QMAKE_LIBDIR += /opt/OceanOptics/OmniDriverSPAM/OOI_HOME
QMAKE_CXXFLAGS += -DLINUX
QMAKE_CXXFLAGS += -DMAXWAVELEGNTH_BUFFER_ELEMTENTS=4096
LIBS += -lqserialdevice
LIBS += -lqwt
LIBS += -lOmniDriver
LIBS += -lcommon
LIBS += -ljvm
LIBS += -L/opt/OceanOptics/OmniDriverSPAM/_jvm/lib/i386/server
SOURCES += main.cpp \
    mainwindow.cpp \
    tfrmtempctrl.cpp \
    qwtsymbolarrow.cpp \
    qwtmarkerarrow.cpp \
    thwdriver.cpp \
    tspectrum.cpp \
    tretrieval.cpp \
    tretrievalimage.cpp \
    tgassignature.cpp \
    crc8.c \
    tfrmspectrconfig.cpp \
    maxdoassettings.cpp
HEADERS += mainwindow.h \
    tfrmtempctrl.h \
    qwtsymbolarrow.h \
    qwtmarkerarrow.h \
    thwdriver.h \
    tspectrum.h \
    tretrieval.h \
    tretrievalimage.h \
    tgassignature.h \
    crc8.h \
    tfrmspectrconfig.h \
    maxdoassettings.h
FORMS += mainwindow.ui \
    tfrmtempctrl.ui \
    tfrmspectrconfig.ui
include(../libs/log4qt/src/log4qt/log4qt.pri)
