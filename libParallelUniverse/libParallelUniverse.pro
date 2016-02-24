#-------------------------------------------------
#
# Project created by QtCreator 2015-11-24T18:34:23
#
#-------------------------------------------------

QT       -= core gui

TARGET = ParallelUniverse
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    ParallelUniverse.cpp

HEADERS += \
    ParallelUniverse.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}



#----------------------------------------------------
LIB_SUT_PATH = ../libsut/
LIB_MMDCPPDEV_PATH = ../libMMDCppDev/
LIB_VISUALIZATIONAVATAR_PATH = ../libVisualizationAvatar/

INCLUDEPATH += $${LIB_SUT_PATH} \
               $${LIB_MMDCPPDEV_PATH} \
               $${LIB_VISUALIZATIONAVATAR_PATH}

LIBS += -L$${LIB_SUT_PATH} -lsut \
        -L$${LIB_MMDCPPDEV_PATH} -lMMDCppDev \
        -L$${LIB_VISUALIZATIONAVATAR_PATH} -lVisualizationAvatar
