#ifndef PSTOUCH_H
#define PSTOUCH_H

#include <stdio.h>
#include <vector>
#include <ctime>
#include <climits>
#include <stdlib.h>
#include <sstream>
#include <QObject>

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/nonfree.hpp"

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <QApplication>
#include <QMouseEvent>
#include <QPoint>
#include <cmath>
#include <QTime>
#include <QDebug>

#include "OpenNI.h"
#include "depthhelper.h"
#include "touchevent.h"
#include "pstransform.h"
#include "OpenNI.h"
#include "planedetector.h"
#include <QThread>


#include <QtDebug>
#include <QGraphicsItem>
#include "tracker.h"

// alternative blob detector
#include "library/blob.h"
#include "library/BlobResult.h"

#define IMAGE_HIST_COUNT 90 // amount of samples for groundtrouth
#define NOISE 5 // noise threashold for dMax in mm  //3,5mm //was 5
#define FINGER 15 // Finger size in mm 11                   // was 11
#define BOXFILTER 5 // size of boxfilter for noise filtering
#define CALIBRATIONFILE "calibration.yaml"
#define NUMCORES 3

class PSTouch : public QThread
{
    Q_OBJECT

private:
    openni::Device device;
    openni::VideoStream* depthStreamPtr;
    openni::VideoStream* rgbStreamPtr;
    openni::VideoMode* depthMode;
    openni::VideoMode* rgbMode;
    DepthHelper* dh;
    cv::Mat touchFilter; // spatial filter for touch
    cv::Mat* groundTruth;
    cv::Mat* normals;
    cv::Mat kernelX;
    cv::Mat kernel;
    cv::Mat kernelY;
    bool calibrated;
    bool running;
    cv::Mat dMax;

    QTime timer;
    int timerCount;
    PSTransform* transform;

    cv::string PixelFormatToString(openni::PixelFormat format);
    void readFrame(cv::Mat *Image,openni::VideoStream* Stream);
    void blobDetect(cv::Mat& image);



public slots:
    void stopTouchDetection();
    void startTouchDetection();
    float getDistance(int x, int y);


signals:
    void touchEvent(int x, int y);
    void view(QGraphicsItem* item);
    void sendTransformation(QTransform t);
    //void touch(cv::Point2i point );
    void updateEvents(std::vector<TouchEvent> events);

public:
    PSTouch();
    ~PSTouch();
    void printInfo(openni::SensorType type);
//    void displayDepthFrame();
    void displayColorFrame();

    void calibrateTouch();
    void detectTouch();
    openni::VideoStream* getDepthStream(){return depthStreamPtr;}
    openni::VideoStream* getColorStream(){return rgbStreamPtr;}

    void run();
    QTransform getUiTransformation(int x, int y);

};

#endif // PSTOUCH_H
