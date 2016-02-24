#-------------------------------------------------
#
# Project created by QtCreator 2015-11-24T14:59:39
#
#-------------------------------------------------

QT       -= core gui

TARGET = MMDCppDev
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    PMD_Profile.cpp \
    PMX_Profile.cpp \
    PostureMemory.cpp \
    VMD_Profile.cpp

HEADERS += \
    PMD_Profile.h \
    PMX_Profile.h \
    PostureMemory.h \
    VMD_Profile.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

#----------------------------------------------------
LIB_SUT_PATH = ../libsut/
INCLUDEPATH += $${LIB_SUT_PATH}
LIBS += -L$${LIB_SUT_PATH}/lib -lsut

