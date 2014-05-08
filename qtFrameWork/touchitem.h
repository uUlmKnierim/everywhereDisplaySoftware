#ifndef TOUCHITEM_H
#define TOUCHITEM_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPainter>
#include "opencv2/opencv.hpp"


class TouchItem: public QObject, public QGraphicsItem
{
Q_OBJECT

    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)


public:
    TouchItem(const cv::Point2i point, QGraphicsScene *scene,  QObject* parent = 0);

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);
public slots:
    void animationFinished();

private:
       cv::Point2i point;
       QGraphicsScene* scene;
};

#endif // TOUCHITEM_H
