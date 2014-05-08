#ifndef PLANEDETECTOR_H
#define PLANEDETECTOR_H

#include <OpenNI.h>
#include "opencv2/opencv.hpp"
#include <QDebug>

class PlaneDetector
{
public:
    PlaneDetector(openni::VideoStream* stream);
    void calculateNormalImage(cv::Mat* image, cv::Mat* normals);

private:
    const openni::VideoStream* depthStream;
    cv::Mat img;
};

#endif // PLANEDETECTOR_H
