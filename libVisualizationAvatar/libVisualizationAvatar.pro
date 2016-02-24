#-------------------------------------------------
#
# Project created by QtCreator 2015-11-24T15:49:10
#
#-------------------------------------------------

QT       -= core gui


TARGET = VisualizationAvatar
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    vaVisualizationFunctions.cpp \
    vaMotionFunctions.cpp \
    vaMemory.cpp \
    va.cpp

HEADERS += \
    vaMemory.h \
    va.h \
    vaPostureMemory.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

INCLUDEPATH += ../libsut/ ../libMMDCppDev/
LIBS += -L../libsut/ -lsut

QMAKE_CXXFLAGS += -O3 -g

#----------------------------------------------------
DEFINES += USEING_QT


contains(DEFINES, USEING_QT) {
    QT      += opengl
}
else {
    QT      -= opengl
}

LIB_SUT_PATH = $$system(pwd)/../../libsut/
LIB_MMDCPPDEV_PATH = $$system(pwd)/../../libMMDCppDev/

INCLUDEPATH += $${LIB_SUT_PATH} \
               $${LIB_MMDCPPDEV_PATH} \


LIBS += -lGLU \
        -L$${LIB_SUT_PATH}/lib/ -lsut \
        -L$${LIB_MMDCPPDEV_PATH}/lib/ -lMMDCppDev \
