#ifndef TOUCHEVENT_H
#define TOUCHEVENT_H

#include "OpenNI.h"
#include "opencv2/opencv.hpp"
#include <QTime>
#include <QEvent>

class TouchEvent
{
public:
    TouchEvent();
    TouchEvent(cv::Point2i pointProjector, cv::Point3f pointPSCam);

    bool tracked;
    cv::Point2i pointProjector;
    cv::Point3f pointPSCam;
    int age;
    int lifes;
    QTime timer;
    bool lpEmit; //LongPress emitted
    bool timerSet;
    QEvent::Type event;
};

#endif // TOUCHEVENT_H
