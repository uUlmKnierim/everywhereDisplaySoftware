#include "pictureframewidget.h"
#include "QSize"
#include "QColor"

PictureFrameWidget::PictureFrameWidget(QWidget *parent)
    : QWidget(parent)
{
    resize(200, 200);

    //Use this property with caution
    setAttribute( Qt::WA_TranslucentBackground,true);
    //TODO:
    //Load image list dynamic ;)
    images.append("/home/linaro/qtBiquitousUI/images/a.jpg");
    images.append("/home/linaro/qtBiquitousUI/images/b.jpg");
    images.append("/home/linaro/qtBiquitousUI/images/c.jpg");
    imageNr = 0;
}



void PictureFrameWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    //painter.translate(width() / 2, height() / 2);
    int side = qMin(width(), height());
    painter.scale(side / 200.0, side / 200.0);
    QString t = images.at(imageNr);
    qDebug() <<"image: " << t;
    QPixmap image(t);
    image = image.scaledToWidth(200);
    painter.drawPixmap(0,0, image);
    if(moving){
        QPen *pen = new QPen(Qt::red);
        pen->setWidth(4);
        painter.setPen(*pen);
        painter.drawRoundedRect(0,5,image.width()-5, image.height()-5,3,3);
    }
}



void PictureFrameWidget::mouseDoubleClickEvent(QMouseEvent *evt)
{
    oldPos = evt->globalPos();
    moving = true;
    repaint();
}

void PictureFrameWidget::mouseMoveEvent(QMouseEvent *evt)
{
  if(moving){
    const QPoint delta = (evt->globalPos() - oldPos);
    move(x()+delta.x(), y()+delta.y());
    oldPos = evt->globalPos();
  }
}


void PictureFrameWidget::mouseReleaseEvent(QMouseEvent *event)
{

  if(moving){
      moving= false;
      repaint();
      return;
  }
    if(event->x()>100){
        imageNr=imageNr +1;
        if(imageNr>2){
            imageNr = 0;
        }
    }
    else{
        imageNr=imageNr - 1;
        if(imageNr<0){
            imageNr = 2;
        }
    }
    qDebug() << "Mouse Released: " << imageNr;
    update();
}


