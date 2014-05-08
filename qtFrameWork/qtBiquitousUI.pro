#-------------------------------------------------
#
# Project created by QtCreator 2014-01-04T12:56:46
#
#-------------------------------------------------

QT       +=core declarative gui phonon webkit network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qtBiquitousUI
TEMPLATE = app


SOURCES += main.cpp\
    pstouch.cpp \
    depthhelper.cpp \
    pstransform.cpp \
    touchevent.cpp \
    server.cpp \
    planedetector.cpp \
    domparser.cpp \
    mainview.cpp \
    widget/analogclock.cpp \
    widget/pictureframewidget.cpp \
    tracker.cpp \
    audiofeedback.cpp \
    touchitem.cpp \
    widget/widgetloader.cpp \
    uitranformer.cpp \
    widget/panwebviewwidget.cpp \
    library/blob.cpp \
    library/BlobContour.cpp \
    library/BlobOperators.cpp \
    library/BlobResult.cpp \
    library/ComponentLabeling.cpp

HEADERS  += \
    pstouch.h \
    OpenNI.h \
    depthhelper.h \
    pstransform.h \
    touchevent.h \
    server.h \
    planedetector.h \
    domparser.h \
    mainview.h \
    widget/analogclock.h \
    widget/pictureframewidget.h \
    tracker.h \
    audiofeedback.h \
    touchitem.h \
    widget/widgetloader.h \
    settings.h \
    uitranformer.h \
    widget/panwebviewwidget.h \
    library/blob.h \
    library/BlobContour.h \
    library/BlobLibraryConfiguration.h \
    library/BlobOperators.h \
    library/BlobResult.h \
    library/ComponentLabeling.h

INCLUDEPATH += /home/linaro/OpenNI-Linux-Arm-2.2/Include

LIBS += -L/usr/local/lib \
     -L/home/linaro/OpenNI-Linux-Arm-2.2/Redist \
     -lOpenNI2 \
     -lopencv_core \
     -lopencv_imgproc \
     -lopencv_features2d \
     -lopencv_highgui \
     -lopencv_calib3d \
     -lopencv_video \
     -lserial

#Debug
#QMAKE_CXXFLAGS += -g


# Hardware specifying flags
QMAKE_CXXFLAGS += -march=armv7-a -mtune=cortex-a15 -mfpu=neon -mcpu=cortex-a8 #  not activ:-mcpu=cortex-a8 ; and -mtune=cortex-a9
# Optimization level, minus currently buggy optimizing methods (which break bit-exact)
QMAKE_CXXFLAGS += -O3 -fno-tree-pre -fno-strict-aliasing
# More optimization flags
QMAKE_CXXFLAGS += -ftree-vectorize -ffast-math -funsafe-math-optimizations #-fsingle-precision-constant
#DEFINES += XN_NEON
#QMAKE_CXXFLAGS += -flax-vector-conversions

OTHER_FILES += \
    calibration.yaml \
    images/image_button_bg.png \
    qmlWidget/photoviewer/photoviewer.qml \
    qmlWidget/bustimetable/QMLBusTimetable.qml \
    qml/photoviewer/* \
    qmlWidget/bustimetable/config.png
