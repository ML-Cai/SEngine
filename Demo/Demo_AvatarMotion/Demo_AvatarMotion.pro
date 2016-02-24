#-------------------------------------------------
#
# Project created by QtCreator 2016-01-26T16:33:20
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Demo_AvatarMotion
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    QGLWidget_AvatarRendering.cpp

HEADERS  += mainwindow.h \
    QGLWidget_AvatarRendering.h

FORMS    += mainwindow.ui

#----------------------------------------------------
DEFINES += USEING_QT

LIB_OPENCV_PATH = /home/lib_install/opencv-3.0.0
LIB_SUT_PATH = $$system(pwd)/../../libsut/
LIB_MMDCPPDEV_PATH = $$system(pwd)/../../libMMDCppDev/
LIB_VISUALIZATIONAVATAR_PATH = $$system(pwd)/../../libVisualizationAvatar/
LIB_PARALLELUNIVERSE_PATH = $$system(pwd)/../../libParallelUniverse/

INCLUDEPATH += $${LIB_SUT_PATH} \
               $${LIB_MMDCPPDEV_PATH} \
               $${LIB_VISUALIZATIONAVATAR_PATH} \
               $${LIB_PARALLELUNIVERSE_PATH}

LIBS += -lGLU \
        -L$${LIB_OPENCV_PATH}/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lopencv_xfeatures2d -lopencv_features2d\
        -L$${LIB_MMDCPPDEV_PATH}/lib/ -lMMDCppDev \
        -L$${LIB_VISUALIZATIONAVATAR_PATH}/lib/ -lVisualizationAvatar \
        -L$${LIB_PARALLELUNIVERSE_PATH}/lib/ -lParallelUniverse \
        -L$${LIB_SUT_PATH}/lib/ -lsut
