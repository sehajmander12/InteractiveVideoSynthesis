#-------------------------------------------------
#
# Project created by QtCreator 2024-10-29T22:48:16
#
#-------------------------------------------------
QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ModalAnalysis
TEMPLATE = app

DYLD_LIBRARY_PATH += $$_PRO_FILE_PWD_/opencv/build/lib

OPENCV_MODULES = highgui core imgcodecs videoio imgproc features2d \
    flann video calib3d ml objdetect photo shape stitching superres \
    videostab
OPENCV_CONTRIB_MODULES = xfeatures2d plot

INCLUDEPATH += analysis simulation \
    $$/open_PRO_FILE_PWD_cv/include \
    $$PWD/opencv/build/ \
    $$PWD/Eigen/src/


for (module, OPENCV_MODULES): INCLUDEPATH += $$PWD/opencv/modules/$$module/include
for (module, OPENCV_CONTRIB_MODULES): INCLUDEPATH += $$PWD/opencv_contrib/modules/$$module/include

LIBS += -L$$_PRO_FILE_PWD_/opencv/build/lib \
    -L$$_PRO_FILE_PWD_/Eigen

for (module, OPENCV_MODULES): LIBS += -lopencv_$$module
for (module, OPENCV_CONTRIB_MODULES): LIBS += -lopencv_$$module
SOURCES += main.cpp \
    analysis/FeatureSelector.cpp \
    analysis/featuredetector.cpp \
    analysis/fileutil.cpp \
    analysis/DelaunyTriangulation.cpp \
    analysis/FeatureTracker.cpp \
    analysis/utility.cpp \
    analysis/ModalShapeProducer.cpp \
    analysis/FrequencySelector.cpp \
    analysis/VideoSelector.cpp \
    analysis/OpenGLWarper.cpp \
    analysis/OpenGLWindow.cpp \
    analysis/ModalShapeDrawer.cpp \
    analysis/ComplexUtility.cpp \
    analysis/DisplayImageWidget.cpp \
    simulation/featurepoint.cpp \
    simulation/readutility.cpp \
    simulation/triangle.cpp \
    simulation/InteractiveWindow.cpp \
    simulation/simulationparams.cpp

HEADERS  += \
    analysis/FeatureSelector.h \
    analysis/featuredetector.h \
    analysis/fileutil.h \
    analysis/DelaunyTriangulation.h \
    analysis/FeatureTracker.h \
    analysis/utility.h \
    analysis/ModalShapeProducer.h \
    analysis/FrequencySelector.h \
    analysis/VideoSelector.h \
    analysis/OpenGLWarper.h \
    analysis/OpenGLWindow.h \
    analysis/ModalShapeDrawer.h \
    analysis/ComplexUtility.h \
    analysis/DisplayImageWidget.h \
    simulation/featurepoint.h \
    simulation/simulationdata.h \
    simulation/simulationparams.h \
    simulation/triangle.h \
    simulation/triangulationdata.h \
    simulation/InteractiveWindow.h

