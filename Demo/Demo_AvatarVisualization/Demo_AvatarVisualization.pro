#-------------------------------------------------
#
# Project created by QtCreator 2015-11-24T22:03:57
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Demo_AvatarVisualization
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    QGLWidget_AvatarRendering.cpp

HEADERS  += mainwindow.h \
    QGLWidget_AvatarRendering.h


#----------------------------------------------------
DEFINES += USEING_QT

LIB_SUT_PATH = $$system(pwd)/../../libsut/
LIB_MMDCPPDEV_PATH = $$system(pwd)/../../libMMDCppDev/
LIB_VISUALIZATIONAVATAR_PATH = $$system(pwd)/../../libVisualizationAvatar/
LIB_PARALLELUNIVERSE_PATH = $$system(pwd)/../../libParallelUniverse/

INCLUDEPATH += $${LIB_SUT_PATH} \
               $${LIB_MMDCPPDEV_PATH} \
               $${LIB_VISUALIZATIONAVATAR_PATH} \
               $${LIB_PARALLELUNIVERSE_PATH}

LIBS += -lGLU \
        -L$${LIB_MMDCPPDEV_PATH}/lib/ -lMMDCppDev \
        -L$${LIB_VISUALIZATIONAVATAR_PATH}/lib/ -lVisualizationAvatar \
        -L$${LIB_PARALLELUNIVERSE_PATH}/lib/ -lParallelUniverse \
        -L$${LIB_SUT_PATH}/lib/ -lsut


