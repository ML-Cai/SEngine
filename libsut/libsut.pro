#-------------------------------------------------
#
# Project created by QtCreator 2015-11-24T14:42:00
#
#-------------------------------------------------

QT       -= core gui

TARGET = sut
TEMPLATE = lib
CONFIG += staticlib

DEFINES += LIBSUT_LIBRARY
QMAKE_CXXFLAGS += -O3 -g

SOURCES += \
    sutBmp.cpp \
    sutLinkList.cpp \
    sutQuaternion.cpp \
    sutQueue.cpp \
    sutHash.c \
    sutBinarySearchTree.cpp \
    sutResourcePool.cpp \
    sutCharacterSetConvert.c

HEADERS += \
    sutAlgebraMath.h \
    sutBmp.h \
    sutDataStruct.h \
    sutHash.h \
    sutLinkList.h \
    sutQuaternion.h \
    sutBinarySearchTree.h \
    sutResourcePool.h \
    sutCharacterSetConvert.h
        libsut_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
