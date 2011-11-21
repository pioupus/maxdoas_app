# #####################################################################
# Automatically generated by qmake (2.01a) Di. Mai 24 21:20:44 2011
# #####################################################################
TEMPLATE = app
include(libs/log4qt/src/log4qt/log4qt.pri)
INCLUDEPATH += . \
    ../diplom_maxdoas/libs/qwt-6.0/src \
    ../diplom_maxdoas/maxdoas \
    ../diplom_maxdoas/libs/qserialdevice/src/qserialdevice \
    ../diplom_maxdoas/libs/qserialdevice/src/qserialdeviceenumerator \
    /home/arne/opt/OmniDriverSPAM-1.66/include \
    /usr/lib/jvm/default-java/include \
    /usr/lib/jvm/default-java/include/linux \
    /home/arne/opt/OmniDriverSPAM-1.66/include
LIBS += -L/home/arne/opt/OmniDriverSPAM-1.66/_jvm/lib/i386/client \
    -ljvm
LIBS += -L/home/arne/opt/OmniDriverSPAM-1.66/OOI_HOME \
    -lOmniDriver \
    -lcommon

# LIBS += -L/home/arne/opt/OmniDriverSPAM-1.66/OOI_HOME -lOmniDriver -lcommon
LIBS += -L../diplom_maxdoas/libs/qwt-6.0/lib \
    -lqwt

# LIBS += -Llibs/qserialdevice/src/build/release -lqserialdevice
LIBS += -ludev
CONFIG(debug, debug|release) { 
    QMAKE_LIBDIR += ../diplom_maxdoas/libs/qserialdevice/src/build/debug
    LIBS += -lqserialdeviced
}
else { 
    QMAKE_LIBDIR += ../diplom_maxdoas/libs/qserialdevice/src/build/release
    LIBS += -lqserialdevice
}
CONFIG += qtestlib
QT += script
QT += scripttools
QT += svg
QT += xml
DEFINES += LINUX

# Input
HEADERS += maxdoas/mainwindow.h \
    maxdoas/qwtmarkerarrow.h \
    maxdoas/qwtsymbolarrow.h \
    maxdoas/tfrmspectrconfig.h \
    maxdoas/tfrmtempctrl.h \
    maxdoas/tgassignature.h \
    maxdoas/thwdriver.h \
    maxdoas/tretrieval.h \
    maxdoas/tretrievalimage.h \
    maxdoas/tspectrum.h \
    maxdoas/maxdoassettings.h \
    maxdoas/bubblewidget.h \
    maxdoas/wavelengthbuffer.h \
    maxdoas/crc8.h \
    maxdoas/tmirrorcoordinate.h \
    maxdoas/scriptwrapper.h \
    jsedit/jsedit.h \
    maxdoas/tspectralimage.h \
    maxdoas/tscanpath.h \
    maxdoas/tspectrumplotter.h \
    maxdoas/qdoaswrapper.h \
    maxdoas/tdirlist.h
FORMS += maxdoas/mainwindow.ui \
    maxdoas/tfrmtempctrl.ui \
    maxdoas/tfrmspectrconfig.ui
SOURCES += maxdoas/main.cpp \
    maxdoas/mainwindow.cpp \
    maxdoas/tfrmspectrconfig.cpp \
    maxdoas/qwtmarkerarrow.cpp \
    maxdoas/qwtsymbolarrow.cpp \
    maxdoas/tfrmtempctrl.cpp \
    maxdoas/tgassignature.cpp \
    maxdoas/thwdriver.cpp \
    maxdoas/tretrieval.cpp \
    maxdoas/tretrievalimage.cpp \
    maxdoas/tspectrum.cpp \
    maxdoas/maxdoassettings.cpp \
    maxdoas/bubblewidget.cpp \
    maxdoas/wavelengthbuffer.cpp \
    maxdoas/crc8.c \
    maxdoas/tmirrorcoordinate.cpp \
    maxdoas/scriptwrapper.cpp \
    jsedit/jsedit.cpp \
    maxdoas/tspectralimage.cpp \
    maxdoas/tscanpath.cpp \
    maxdoas/tspectrumplotter.cpp \
    maxdoas/qdoaswrapper.cpp \
    maxdoas/tdirlist.cpp
