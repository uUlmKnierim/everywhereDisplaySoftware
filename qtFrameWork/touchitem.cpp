#include "touchitem.h"
#include <QPropertyAnimation>
#include <QEasingCurve>

#define SIZE 10

TouchItem::TouchItem(const cv::Point2i point, QGraphicsScene *scene, QObject *parent)
{
        this->point = point;
        this->scene= scene;
        this->scene->addItem(this);

        //this->setOpacity();
        //this->opacity()

        // Start animate this class
        QPropertyAnimation* anim = new QPropertyAnimation(this, "opacity");
        // 2 second duration animation
        anim->setDuration(2000);
        // position to start animation
        anim->setStartValue(1);
        // end position of animation
        anim->setEndValue(0);
        // easing curve
        QObject::connect(anim, SIGNAL(finished()), this, SLOT(animationFinished()));

        anim->setEasingCurve(QEasingCurve::InOutElastic);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
}

QRectF TouchItem::boundingRect() const
{
    return QRectF(point.x-(SIZE/2),point.y-(SIZE/2),SIZE,SIZE);
}

void TouchItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setBrush(Qt::green);
    painter->drawEllipse(point.x-(SIZE/2),point.y-(SIZE/2),SIZE,SIZE);
}

void TouchItem::animationFinished()
{
    scene->removeItem(this);
    delete this;
}
