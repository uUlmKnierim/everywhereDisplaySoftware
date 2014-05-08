#include "pstransform.h"
#include "opencv2/calib3d/calib3d.hpp"


using namespace cv;

PSTransform::PSTransform(openni::VideoStream* dstream,openni::VideoStream* cstream,string file)
{
 readCalibFile(file);
 this->dstream = dstream;
 this->cstream = cstream;
}

PSTransform::~PSTransform(){

}

// Returns first Element of a FileStorageNode with @name;
string PSTransform::readNode(FileStorage fs, string name){
    FileNode n = fs[name];
    if (n.type() != FileNode::SEQ)
    {
      std::cerr << "Strings is not a sequence! FAIL" << std::endl;
    }
    FileNodeIterator it = n.begin();// Go through the node
    return (string)*it;
}

void PSTransform::readCalibFile(string file){
    std::cout << "Start Reading Yamal File" << std::endl;;
    FileStorage fs(file, FileStorage::READ);

    string myCam = readNode(fs, "Cameras");
    string myProjector = readNode(fs, "Projectors");

    // Read CameraParameter
    FileNode cameraNode = fs[myCam];
    // load cam matrices
    cameraNode["cameraMatrix"] >> cameraMatrix;
    cameraNode["distortionCoeffs"] >> cameraDistortionCoeffs;
    cameraNode["extrParams"] >> cameraExtrParams;
    cameraNode["R"] >> cameraR;
    cameraNode["T"] >> cameraT;
    cameraNode["E"] >> cameraE;
    cameraNode["F"] >> cameraF;

    // read projector parameter
    FileNode projectorNode = fs[myProjector];
    // load projector matrices
    projectorNode["cameraMatrix"] >> projectorMatrix;
    projectorNode["distortionCoeffs"] >> projectorDistortionCoeffs;
    projectorNode["extrParams"] >> projectorExtrParams;
    projectorNode["R"] >> projectorR;
    projectorNode["T"] >> projectorT;
    projectorNode["E"] >> projectorE;
    projectorNode["F"] >> projectorF;

    // Projector Intrinsics
    projectorMatrix.convertTo(projectorIntrinsic, CV_32F);
    std::cout << "projectorIntrinsic:\n" << projectorIntrinsic << std::endl;

    // Projector Extrinsics
    projectorR.convertTo(projectorRot, CV_32F);
    projectorT.convertTo(projectorTrans, CV_32F);

    std::cout << "projectorR:\n" << projectorRot <<"\n Trans:" <<projectorTrans << std::endl;
}


cv::Point2f PSTransform::transformPointfromCamToProjector(Point3f point){


    // first undisrot opserved point
    vector<Point2f> observedpt;
    Point2f observedTouchPoint;
    observedTouchPoint.x = point.x;
    observedTouchPoint.y = point.y;
    observedpt.push_back(observedTouchPoint);
    undistortPoints(observedpt,observedpt,cameraMatrix,cameraDistortionCoeffs);

    // transform to 3d World
    Mat observedHomo = (Mat_<double>(3,1) << observedpt[0].x, observedpt[0].y, 1 );
    Mat worldPoint = observedHomo * point.z;
//    std::cout << worldPoint << "<- cv transform"<< std::endl;
    Mat worldInPixel = worldPoint * depthTOpixel;

    Point3f tpoint;
    tpoint.x = worldInPixel.at<double>(0, 0);///cccam.at<float>(0, 2); // GGF Änder.
    tpoint.y = worldInPixel.at<double>(0, 1);///cccam.at<float>(0, 2);
    tpoint.z = worldInPixel.at<double>(0, 2);


    // undistrot
    vector<Point3f> pt0;
    pt0.push_back(tpoint);

    Point2f pt2f;
    vector<Point2f> pt1;
    pt1.push_back(pt2f);

    Mat rvec;
    cv::Rodrigues(projectorR, rvec);

    //project from world to projector
    projectPoints(pt0,rvec,projectorT, projectorMatrix, projectorDistortionCoeffs,pt1);

    return pt1[0];
}

Vec3f PSTransform::transformVectorfromCamtoProjector(Vec3f vec)
{
    Mat t = (Mat_<float>(3,1) << vec[0],vec[1] , vec[2] );
    t =  projectorRot.inv() * t;
    return Vec3f(t.at<float>(0,0),t.at<float>(1,0),t.at<float>(2,0));
}


// returns World coordinate in pixel
Vec3f PSTransform::transformPointFromCamToWorld(Vec3f vec)
{

    // first undisrot opserved point
    vector<Point2f> observedpt;
    Point2f observedTouchPoint;
    observedTouchPoint.x = vec[0];
    observedTouchPoint.y = vec[1];
    observedpt.push_back(observedTouchPoint);
    undistortPoints(observedpt,observedpt,cameraMatrix,cameraDistortionCoeffs);

    // transform to 3d World
    Vec3f observedHomo = Vec3f(observedpt[0].x, observedpt[0].y, 1);
    cv::Vec3f res =  observedHomo * vec[2] * depthTOpixel;
    qDebug() << "res "<< res[0]<< " " << res[1]<< " " << res[2] <<" " ;
    return res;
}

Point2f PSTransform::transformWorldPointToProjector(Point3f point)
{
    // undistrot
    vector<Point3f> pt0;
    pt0.push_back(point);
    Point2f pt2f;
    vector<Point2f> pt1;
    pt1.push_back(pt2f);
    Mat rvec;
    cv::Rodrigues(projectorR, rvec);

    //project from world to projector
    projectPoints(pt0,rvec,projectorT, projectorMatrix, projectorDistortionCoeffs,pt1);
    return pt1[0];
}


