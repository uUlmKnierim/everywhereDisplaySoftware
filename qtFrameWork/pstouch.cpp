#include "pstouch.h"

#define PI 3.14159265

using namespace openni;

PSTouch::PSTouch()
{
    const SensorInfo *info;
    depthStreamPtr = new VideoStream;
    rgbStreamPtr = new VideoStream;
    depthMode=0;
    running = true;
    dh = new DepthHelper;
    calibrated = false;
    timerCount = 0;
    kernel = cv::Mat::ones( BOXFILTER, BOXFILTER, CV_32F )/ (float)(BOXFILTER*BOXFILTER);
    kernelX = cv::Mat::ones(1,BOXFILTER,CV_32F)/(float)BOXFILTER;
    kernelY = cv::Mat::ones(BOXFILTER,1,CV_32F)/(float)BOXFILTER;
    try {
        OpenNI::initialize();

        if (device.open(openni::ANY_DEVICE) != openni::STATUS_OK) {
            std::cout << "No PrimeSense (Xtion) device found | Exit" << std::endl;
            // TODO: Throw exceptinon
        }

        depthStreamPtr->create(device, openni::SENSOR_DEPTH);
        rgbStreamPtr->create(device, openni::SENSOR_COLOR);

        // Depth
        printf("Checking possible Modes\n");
        if(device.hasSensor(SENSOR_DEPTH)){
            info = device.getSensorInfo(SENSOR_DEPTH);
            const Array< VideoMode > &  modes = (*info).getSupportedVideoModes();
//#ifdef PRINT
            for ( int i = 0; i < modes.getSize(); i++ )
            {
                int x = modes[i].getResolutionX();
                int y = modes[i].getResolutionY();
                int fps = modes[i].getFps();
                const PixelFormat format = modes[i].getPixelFormat();
                cv::string strFormat = PixelFormatToString(format);
                printf("%d. Resolution: %d x %d @ %dfps - %s\n",i, x,y,fps, strFormat.c_str());
            }
//#endif
            depthMode = new VideoMode(modes[4]);
            depthStreamPtr->setVideoMode(*depthMode);
            depthStreamPtr->setMirroringEnabled(false);
        }
        //info.getSupportedVidoeModes();
        printf("Done getting VideoModes(depth)\n");


        // RGB
        printf("Checking possible Modes(RGB)\n");
        if(device.hasSensor(SENSOR_COLOR)){
            info = device.getSensorInfo(SENSOR_COLOR);
            const Array< VideoMode > &  modes = (*info).getSupportedVideoModes();

//    #ifdef PRINT
            for ( int i = 0; i < modes.getSize(); i++ )
            {
                int x = modes[i].getResolutionX();
                int y = modes[i].getResolutionY();
                int fps = modes[i].getFps();
                const PixelFormat format = modes[i].getPixelFormat();
                cv::string strFormat = PixelFormatToString(format);
                printf("%d. Resolution: %d x %d @ %dfps - %s\n",i, x,y,fps, strFormat.c_str());
            }
//    #endif
            rgbMode = new VideoMode(modes[9]);
            rgbStreamPtr->setVideoMode(*rgbMode);
            rgbStreamPtr->setMirroringEnabled(false);
        }
        printf("Done getting VideoModes(rgb)\n");

        if (depthStreamPtr->start() != openni::STATUS_OK)
        {
            std::cout << "Stream.start() != openni::STATUS_OK" << std::endl;
        }

        if (rgbStreamPtr->start() != openni::STATUS_OK)
        {
            std::cout << "Stream.start() != openni::STATUS_OK : "<< std::endl;
        }
        printf("Streams started\n");
    } catch (...) {
          std::cout << "Errors occure" << std::endl;
    }
    device.setImageRegistrationMode(IMAGE_REGISTRATION_DEPTH_TO_COLOR);

    transform = new PSTransform(depthStreamPtr,rgbStreamPtr,CALIBRATIONFILE);

}

PSTouch::~PSTouch(){
    delete depthMode;
    delete rgbMode;
    delete depthStreamPtr;
    delete rgbStreamPtr;
    delete transform;
    delete dh;
    delete normals;
}

void PSTouch::startTouchDetection(){
    std::cout << "SLOT - PSTOUCH: start cam" << std::endl;
    this->calibrateTouch();
    if(!running){
      running = true;
      this->start();
    }
}

void PSTouch::stopTouchDetection(){
    std::cout << "SLOT - PSTOUCH: stop cam" << std::endl;
    running = false;
}

float PSTouch::getDistance(int x, int y)
{
    cv::Mat img;
    readFrame(&img,getDepthStream());
    float distance = img.at<openni::DepthPixel>(y,x)/(float)10;
    std::cout << "Slot: getDistance - Result: " << distance <<  std::endl;
    return distance;
}



void PSTouch::readFrame(cv::Mat* Image,VideoStream* Stream){
    SensorType type = Stream->getSensorInfo().getSensorType();
    int frameType = CV_16U;

    if(type == SENSOR_COLOR){
        frameType = CV_8UC3;
    }else if (type == SENSOR_DEPTH){
        frameType = CV_16U;
    }
    std::vector < openni::VideoStream * >streams;
    streams.push_back(Stream);

    int changedIndex;
    openni::OpenNI::waitForAnyStream(&streams[0], streams.size(), &changedIndex);

    openni::VideoFrameRef Frame;
    Stream->readFrame(&Frame);

    if (Frame.isValid()) {
        (*Image) = cv::Mat(   (*Stream).getVideoMode().getResolutionY(),
                           (*Stream).getVideoMode().getResolutionX(),
                           frameType,
                           (char *) Frame.getData());
        //depends on how camera is mounted
        // 0 = vertical flip
        // 1 = horizontal flip
        //-1 = both flip
        //--> image should look like a photo of scenery.
//        flip(*Image, *Image, -1);
//        dh->displayDepthImage(*Image,"DepthImage on Read");
    }
    else{
        printf("failed to read\n");
    }
}

void PSTouch::printInfo(openni::SensorType type){
    openni::VideoMode* videoMode = 0;
    if(type == SENSOR_COLOR){
        std::cout << "Colorstream Information:" << std::endl;
        videoMode = rgbMode;
    }else if (type == SENSOR_DEPTH){
        std::cout << "Depthstream Information:"<< std::endl;
        videoMode = depthMode;
    }
    else{
        std::cout << "No Stream found | Exit" << std::endl;
    }
    int x = videoMode->getResolutionX();
    int y = videoMode->getResolutionY();
    int fps = videoMode->getFps();
    const PixelFormat format = videoMode->getPixelFormat();
    cv::string strFormat = PixelFormatToString(format);
    printf("Resolution: %d x %d @ %dfps - %s\n", x,y,fps, strFormat.c_str());
}


//void PSTouch::displayDepthFrame(){
//    cv::Mat im;
//    readFrame(&im, depthStreamPtr);
//    dh->displayDepthImage(im,"DepthImage");
//}

void PSTouch::displayColorFrame(){
    cv::Mat im;
    readFrame(&im, rgbStreamPtr);
    cv::imshow("ColorImage", im);
}

void PSTouch::calibrateTouch(){

    groundTruth = new cv::Mat( cv::Mat::zeros(depthMode->getResolutionY(),
                                 depthMode->getResolutionX(),CV_32S));
    cv::Mat tmp;
    cv::Mat thres;
    cv::Mat denominator = cv::Mat::zeros(depthMode->getResolutionY(),depthMode->getResolutionX(),CV_32S);

    for(int i = 0; i<IMAGE_HIST_COUNT;i++){
        readFrame(&tmp,depthStreamPtr);

        erudeDepth(&tmp);

        tmp.convertTo(thres, CV_32F);
        threshold(thres, thres, 1, 1, 0);
        thres.convertTo(thres,CV_32S);

        tmp.convertTo(tmp, CV_32S);
        *groundTruth = *groundTruth + tmp;

        denominator = denominator + thres;
    }

    *groundTruth = *groundTruth/denominator;

    groundTruth->convertTo(*groundTruth, CV_16U);
    normals = new cv::Mat( cv::Mat::zeros(depthMode->getResolutionY(),
                                              depthMode->getResolutionX(), CV_32FC3));


    PlaneDetector planeDetector(depthStreamPtr);
    planeDetector.calculateNormalImage(groundTruth, normals);

    // Spatial filtering
    touchFilter = groundTruth->clone();
    touchFilter.convertTo(touchFilter, CV_8U);

    calibrated = true;
    dMax = groundTruth->clone();// cv::Mat(*groundTruth);
    dMax = dMax * 0.99;
}




void PSTouch::detectTouch(){

    cv::Mat image;
    readFrame(&image, depthStreamPtr);

    //TODO: Change result image to 8bit;
    if(!calibrated){
        printf("callibrate First");
    }
    erudeDepth(&image);
    image = dMax - image;
    // Create Binary image 256(touch) 0(no touch)
    cv::MatIterator_<openni::DepthPixel> it = image.begin<openni::DepthPixel>(), it_end = image.end<openni::DepthPixel>();
    for(; it != it_end; ++it){
        if((*it)>0 && (*it) <FINGER){
            *it= (openni::DepthPixel)(256);
        }
        else{
            *it= (openni::DepthPixel)0;
        }
    }

    //Creater Boxfilter and filter + thresholding the image.
    image.convertTo(image, CV_8U);
    //cv::imshow("binary", image);
    //timer.restart();
    cv::Mat tmp;
    sepFilter2D(image,tmp,-1,kernelX,kernelY);
    image = tmp;
    //int x = timer.restart();
    //qDebug() <<x << "ms for sep";
    //timer.restart();
    //filter2D(image, image, -1 , kernel , cv::Point(-1,-1), 0, cv::BORDER_DEFAULT );
    //x = timer.restart();
    //qDebug() <<x << "ms for 2d";

    // Spatial filtering: not really a option :/ ?
    touchFilter = 0.15* touchFilter + 0.85*image;
    //cv::imshow("touchFilter",touchFilter);

    cv::threshold( touchFilter, touchFilter, 205, 255, 3);  // shuld not be fixed
    //cv::imshow("Touch",touchFilter);

    blobDetect(touchFilter);
}

void PSTouch::run()

{
 timer.start();
    while(running){
      this->detectTouch();
    }
}

Q_DECLARE_METATYPE (std::vector<TouchEvent>)

void PSTouch::blobDetect(cv::Mat& image){

     CBlobResult res(image,cv::Mat(),NUMCORES);

     std::vector<TouchEvent> events;
     qRegisterMetaType<std::vector<TouchEvent > >("std::vector<TouchEvent>");

    for (unsigned int i = 0; i<res.GetNumBlobs(); i++){
         CBlob blob = res.GetBlob(i);
         if(blob.Area()<3) {
             continue;
           }
         cv::Point point = blob.getCenter();
         cv::Point3f camPoint(point.x,point.y,groundTruth->at<openni::DepthPixel>(point.y,point.x));

         cv::Point2i p = transform->transformPointfromCamToProjector(camPoint);
         if(p.x < 1200 && p.y < 700 && p.x>0 && p.y >0){
             TouchEvent event(p,camPoint);
             events.push_back(event);
         }
     }


     if(events.size() >10){
      qDebug("RECALIBRATE");
      calibrateTouch();
    }

    emit updateEvents(events);


    //Timing
    timerCount++;
    //qDebug()<<"Timer: "<< timerCount;
    if(timerCount==60){
        timerCount=0;
        int x  = timer.restart();
        float fps = 50.0/((float)x/1000.0);
        qDebug() << " working with: " << fps << "fps " << x;
        timer.restart();
    }
}

//Returns the average normal of the 10 times 10 grid based on the normals
//image which is calculated from the ground truth in the calibration step
// x and y are in PSCam cord system
QTransform PSTouch::getUiTransformation(int x, int y){

  qDebug() << "calc normal trans for x:  " << x << "y:" << y;

    //int i = x-5;
    //int j = y-5;
    //int count = 0;
    cv::Vec3f normal(0,0,0);

    //if(x < 5 ) i = 0;
    if(x > 630) x = 630;
    //if(y < 5 ) y = 0;
    if(y > 470) y = 470;
    //int jtmp = j;
    for(int i = x; i<x+10; i++){
        for(int j = y; j<y+10; j++){
            normal  += normals->at<cv::Vec3f>(j,i);
        }
    }
    //cv::imshow("bam",*normals);
    normal = normal/cv::norm(normal);
    std::cout << "avg normal: " << normal<< std::endl;

    //Normal is Z
    // TODO (MAYBE) adapt z to orientation of cam system!
    cv::Vec3f zVec(0,0,1);

    //transform this to projector

    cv::Vec3f projectorNormal = transform->transformVectorfromCamtoProjector(normal);
    cv::Vec3f xVec = normal.cross(zVec);
    cv::Vec3f yVec = normal.cross(xVec);

    qDebug()<< normal[0] <<" "<<normal[1]<<" "<<normal[2] << "\n and \n"
                        << projectorNormal[0] << " " << projectorNormal[1] << " "<< projectorNormal[2] ;

    float xn = projectorNormal[0];
    float yn = projectorNormal[1];
    float zn = projectorNormal[2];

    float xrad = asin(xn);
    float yrad = asin(yn);
    float zrad = asin(zn);

    float xAngle = xrad * 180.0/PI;
    float yAngle = yrad * 180.0/PI;
    float zAngle = zrad * 180.0/PI;

    qDebug() << "xANGLE: " << xAngle;
    qDebug() << "yANGLE: " << yAngle;
    qDebug() << "zANGLE: " << zAngle;

    // Normalize
    xVec = xVec/cv::norm(xVec);
    yVec = yVec/cv::norm(yVec);


            if(xVec[0]<0){
                qDebug("Filp X");
                xVec = xVec *(-1);
            }

            if(yVec[1]>0){
                qDebug("Filp Y");
                yVec = yVec * (-1);
            }

       qDebug() << "xVec: " << xVec[0] << " "<< xVec[1] << " "<< xVec[2];
       qDebug() << "yVec: " << yVec[0] << " "<< yVec[1] << " "<< yVec[2];

       //Let xVec Point exactly down !
       float lamda = -xVec[1]/yVec[1];
       qDebug()<< "lamda: " << lamda;

       float lamda2 = -yVec[0]/xVec[0];

       cv::Vec3f tmp  = xVec + lamda*yVec;

       yVec = yVec + lamda2*xVec;
       xVec = tmp;
       xVec = xVec/cv::norm(xVec);
       yVec = yVec/cv::norm(yVec);

       qDebug() << "RESULT: \nxVec: " << xVec[0] << " "<< xVec[1] << " "<< xVec[2];

       qDebug() << "yVec: " << yVec[0] << " "<< yVec[1] << " "<< yVec[2];


       short depthTmp = groundTruth->at<openni::DepthPixel>(y,x);
       qDebug() << "depth" << depthTmp;
       cv::Vec3f pV(x,y,(float)depthTmp);

    // transform pV (in from cam in World (pixel)
    cv::Vec3f worldPoint = transform->transformPointFromCamToWorld(pV);
//    qDebug() << "CamPoint" << pV[0] << " "<< pV[1] << " "<< pV[2];
//    qDebug() <<"worldPoint" << worldPoint[0] << " "<< worldPoint[1] << " "<< worldPoint[2];


    // fixing Vector orientation:

    if(std::abs(xVec[0])<std::abs(yVec[0])){
        qDebug("FLIP VECTORS");
        cv::Vec3f tmp(xVec);
        xVec = yVec;
        yVec = tmp;
    }

    if (xVec[0] < 0){
        xVec = -1 * xVec;
    }

    if(yVec[1] > 0 ){
        yVec = -1 * yVec;
    }


//    qDebug() << "WORKING WITH: ";
//    qDebug() << "xVec: " << xVec[0] << " "<< xVec[1] << " "<< xVec[2];
//    qDebug() << "yVec: " << yVec[0] << " "<< yVec[1] << " "<< yVec[2];


    // Create Trasnformation
    //
    cv::Point2i p0 = transform->transformWorldPointToProjector(worldPoint);
    cv::Point2i p1 = transform->transformWorldPointToProjector(worldPoint + 400* xVec);
    cv::Point2i p2 = transform->transformWorldPointToProjector(worldPoint + 400* xVec - 400* yVec);
    cv::Point2i p3 = transform->transformWorldPointToProjector(worldPoint - 400* yVec);

        QPolygon base;
        base.setPoints(4, 0,0, 100,0, 100,100, 0,100);

        QPolygon world;
        world.setPoints(4, p0.x,p0.y, p1.x,p1.y, p2.x,p2.y, p3.x,p3.y);

        QTransform transformation;
        bool success = false;
        success = QTransform::quadToQuad(base, world, transformation);

        if(!success)
            qDebug("ERROR: Transformation could not be calculated");

    return transformation;
}


cv::string PSTouch::PixelFormatToString(PixelFormat format){
    switch(format){
        case 100:  return("PIXEL_FORMAT_DEPTH_1_MM");
        case 101:  return("PIXEL_FORMAT_DEPTH_100_UM");
        case 102:  return("PIXEL_FORMAT_SHIFT_9_2");
        case 103:  return("PIXEL_FORMAT_SHIFT_9_3");
        case 200:  return("PIXEL_FORMAT_RGB888");
        case 201:  return("PIXEL_FORMAT_YUV422");
        case 202:  return("PIXEL_FORMAT_GRAY8");
        case 203:  return("PIXEL_FORMAT_GRAY16");
        case 204:  return("PIXEL_FORMAT_JPEG");
        default:   return("Unknown");
    }
}
