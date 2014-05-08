#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QObject>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QPen>
#include <QColor>
#include <QDebug>
#include <iostream>
#include <QTransform>
#include "opencv2/opencv.hpp"
#include <QGraphicsSceneMouseEvent>
#include <QEvent>

#include <QDesktopWidget>
#include <QtAlgorithms>
#include <QGraphicsPathItem>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QMouseEvent>
#include <QGraphicsObject>
#include <QPointF>
#include <QGraphicsSceneMouseEvent>
#include <QEvent>
#include "pstouch.h"
#include "touchevent.h"


#include "widget/pictureframewidget.h"
#include "widget/analogclock.h"
#include "widget/widgetloader.h"


class MainView: public QObject
{
Q_OBJECT

public:
    MainView(PSTouch* transformer);

private:
    QGraphicsScene* scene;
    QGraphicsView* view;
    PSTouch* transformer;
    QGraphicsPixmapItem* image;
    QTransform transformation;


public slots:
    void showMoving();
    void doneMoving();
    void touchEvent(int x, int y);
    void blob(cv::Point2i point);
    void viewItem(QGraphicsItem* item);
    void transform(QTransform t);
    void receiveEvent(TouchEvent touchEvent);
    void setWidget(QString widget , QTransform transformation);
};

#endif // MAINVIEW_H
