#include "touchevent.h"

TouchEvent::TouchEvent(cv::Point2i pointProjector, cv::Point3f pointPSCam)
{
    this->pointProjector = pointProjector;
    this->pointPSCam = pointPSCam;
    age = 1;
    lifes = 5;
    timerSet = false;
    lpEmit = false;
}
