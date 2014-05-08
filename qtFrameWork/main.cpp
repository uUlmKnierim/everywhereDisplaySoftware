#include <QApplication>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include <QGraphicsSceneMouseEvent>

#include "pstouch.h"
#include "pstransform.h"
#include "server.h"
#include "planedetector.h"
#include "domparser.h"
#include "mainview.h"
#include "audiofeedback.h"

#include <QtDebug>

int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    //for sound ;)
    a.setApplicationName( "Mu" );

    //AudioFeedback* audioFeedback = new AudioFeedback;


    DomParser parser;
    Server* server = new Server(&parser);

    //Save new surfaces to xml File
    QObject::connect(server,SIGNAL(save(QString,int,int)),&parser,SLOT(saveNewSurface(QString,int,int)),Qt::DirectConnection);


    PSTouch* touch = new PSTouch;
    Tracker* tracker = new Tracker;


    QObject::connect(server,SIGNAL(getDistance(int,int)),touch,SLOT(getDistance(int,int)));


    MainView* view = new MainView(touch);
    QObject::connect(server,SIGNAL(moving()),view,SLOT(showMoving()));

    //Start and stop cam depending on server connection
    QObject::connect(server,SIGNAL(moving()),touch,SLOT(stopTouchDetection()));
    QObject::connect(server,SIGNAL(disconnected()),view, SLOT(doneMoving()));
    QObject::connect(server,SIGNAL(disconnected()),touch,SLOT(startTouchDetection()));
    // Recalibrate on request
    QObject::connect(server,SIGNAL(calibrate()),touch,SLOT(startTouchDetection()));
    QObject::connect(touch,SIGNAL(touchEvent(int,int)),view,SLOT(touchEvent(int,int)));

    //TESING:
    QObject::connect(touch,SIGNAL(view(QGraphicsItem*)),view,SLOT(viewItem(QGraphicsItem*)));
    QObject::connect(touch,SIGNAL(sendTransformation(QTransform)),view,SLOT(transform(QTransform)));




    // Tracking
    QObject::connect(touch,SIGNAL(updateEvents(std::vector<TouchEvent>)),tracker,SLOT(update(std::vector<TouchEvent>)));
    // emiting touch event
    QObject::connect(tracker,SIGNAL(sendEvent(TouchEvent)),view,SLOT(receiveEvent(TouchEvent)));

    //QObject::connect(tracker,SIGNAL(sendEvent(cv::Point2i,QEvent)),audioFeedback,SLOT(mouseEvent(cv::Point2i,QEvent)));

    //TESTING OFF
    //QObject::connect(touch,SIGNAL(touch(cv::Point2i)),view,SLOT(blob(cv::Point2i)));

    //QObject::connect(touch,SIGNAL(touch(cv::Point2i)),kpf,SLOT(onBlob(cv::Point2i)));



    touch->printInfo(openni::SENSOR_COLOR);
    touch->printInfo(openni::SENSOR_DEPTH);
    touch->calibrateTouch();
    qDebug() << "hello from MAIN thread " << a.thread()->currentThreadId();
    touch->start();

    return a.exec();
}

