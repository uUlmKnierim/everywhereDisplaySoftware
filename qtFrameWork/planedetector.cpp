#include "planedetector.h"

#define INTERVAL 2  //intervall always > Step!!!!! otherwise seg fault ;)
#define STEP 12
cv::Mat img;
PlaneDetector::PlaneDetector(openni::VideoStream* stream){
    depthStream = stream;
    img = cv::Mat(stream->getVideoMode().getResolutionY(), stream->getVideoMode().getResolutionX(), CV_8UC3);

}

// Attention: 16bit input image!!!
void PlaneDetector::calculateNormalImage(cv::Mat* image,cv::Mat* normals){

    if(image->rows!= normals->rows || image->cols!=normals->cols){
         qDebug()<< "Error: wrong cols/rows. Check x/y missmatch!";
    }

    int width = image->cols;
    int height = image->rows ;

    cv::Point3f temp_vectorA; //Top<->Bottom
    cv::Point3f temp_vectorB; // Left<->Right
    cv::Point3f tmpA;
    cv::Point3f tmpB;


    for(int h=0+STEP; h < height-INTERVAL-STEP; h=h+INTERVAL){
        for(int w=0+STEP; w < width-INTERVAL-STEP; w=w+INTERVAL){

    //top
    short depthZ = image->at<short>(h-STEP,w);
    if(depthZ==0) continue;
    openni::CoordinateConverter::convertDepthToWorld(*depthStream,w,h-STEP,depthZ, &tmpA.x, &tmpA.y, &tmpA.z);
    //bottom
    depthZ = image->at<short>(h+STEP,w);
    if(depthZ==0) continue;
    openni::CoordinateConverter::convertDepthToWorld(*depthStream,w,h+STEP,depthZ, &tmpB.x, &tmpB.y, &tmpB.z);
    temp_vectorA = tmpA-tmpB;

    //left
    depthZ = image->at<short>(h,w-STEP);
    if(depthZ==0) continue;
    openni::CoordinateConverter::convertDepthToWorld(*depthStream,w-STEP,h,depthZ, &tmpA.x, &tmpA.y, &tmpA.z);

    //right
    depthZ = image->at<short>(h,w+STEP);
    if(depthZ==0) continue;
    openni::CoordinateConverter::convertDepthToWorld(*depthStream,w+STEP,h,depthZ, &tmpB.x, &tmpB.y, &tmpB.z);
    temp_vectorB = tmpA-tmpB;

    cv::Point3f normal = temp_vectorA.cross(temp_vectorB);
    cv::Mat normalMat = (cv::Mat_<float>(3, 1) << normal.x, normal.y, normal.z);

    double norm = cv::norm(normalMat);

    cv::Mat final = normalMat/norm;

    // 2ed super dirty hack :/
    normals->at<cv::Vec3f>(h,w)[0] =(final.at<float>(0,0));
    normals->at<cv::Vec3f>(h,w)[1] =(final.at<float>(1,0));
    normals->at<cv::Vec3f>(h,w)[2] =(final.at<float>(2,0));
    // ONLY DEBUG
    // Generate Output image
    img.at<cv::Vec3b>(h,w)[0] =(unsigned char)(128+128*final.at<float>(0,0));
    img.at<cv::Vec3b>(h,w)[1] =(unsigned char)(128+128*final.at<float>(1,0));
    img.at<cv::Vec3b>(h,w)[2] =(unsigned char)(128+128*final.at<float>(2,0));
    }
 }
}