#ifndef PSTRANSFORM_H
#define PSTRANSFORM_H

#include "opencv2/opencv.hpp"
#include "OpenNI.h"
#include "touchevent.h"

#include <QDebug>

#define depthTOpixel 200/34 //Pixel per Barcode / mm per Barcode


class PSTransform
{
private:
    openni::VideoStream* dstream;
    openni::VideoStream* cstream;


    cv::Mat projectorDistortionCoeffs, projectorExtrParams, projectorR, projectorT, projectorE, projectorF;
    cv::Mat cameraMatrix, cameraDistortionCoeffs, cameraExtrParams, cameraR, cameraT, cameraE, cameraF;
    cv::Mat projectorIntrinsic, projectorExtrinsics, projectorRot, projectorTrans;

    // Mapping stuff
    cv::Mat rt; // rotation concat translation 3x4 matrix
    cv::Mat projectorMatrix; // Intrisic projector parameter

    void readCalibFile(cv::string file);
    cv::string readNode(cv::FileStorage fs, cv::string name);

public:
    PSTransform(openni::VideoStream* dstream, openni::VideoStream* cstream, cv::string file);
    ~PSTransform();

    cv::Point2f transformPointfromCamToProjector(cv::Point3f point);
    cv::Vec3f transformVectorfromCamtoProjector(cv::Vec3f vec);
    cv::Vec3f transformPointFromCamToWorld(cv::Vec3f vec);
    cv::Point2f transformWorldPointToProjector(cv::Point3f point);
};

#endif // PSTRANSFORM_H
