#include "mainview.h"
#include "touchitem.h"
#include "widget/panwebviewwidget.h"
#include <QDeclarativeView>


// set screen resolution
#define SCREENWIDHT 1200
#define SCREENHEIGHT 700

//PanWebViewWidget web;
//web.setUrl(QUrl("http://news.google.com"));

MainView::MainView(PSTouch *transformer)
{
  qDebug("Create Main View");
  scene = new QGraphicsScene();
  view = new QGraphicsView();
  scene->setSceneRect(0,0,SCREENWIDHT,SCREENHEIGHT);
  scene->setBackgroundBrush(Qt::black);
  view->setScene(scene);
  view->setSceneRect(0,0,SCREENWIDHT,SCREENHEIGHT);
  view->showFullScreen();
  image = NULL;
  this->transformer= transformer;
}

void MainView::showMoving()
{
    //TODO: Performace change visibility or create one Object which stays in memory,
    //don't creater new object every time !

    if(image == NULL ){
        qDeleteAll( scene->items() );
        scene->clear();
        qDebug() << "Show Moving";
        const QPixmap map("images/image_button_bg.png");
        image = new QGraphicsPixmapItem;
        image->setPixmap(map);
        image->setPos(SCREENWIDHT/2-image->boundingRect().center().rx(),
                      SCREENHEIGHT/2-image->boundingRect().center().ry());
        scene->addItem(image);
    }
}

void MainView::doneMoving()
{
    std::cout << "Remove Moving Screen" << std::endl;
    qDeleteAll( scene->items() );
    scene->clear();
    std::cout << "Remove Moving DONE!"<< std::endl;
}

void MainView::touchEvent(int x, int y)
{
    QGraphicsEllipseItem* itemptr = new QGraphicsEllipseItem(x,y,15,15,0);
    QPoint touchPoint(x,y);
    QGraphicsItem *eventItem =  scene->itemAt(x,y);

    if(eventItem!=0){
        QPointF mappedPoint = eventItem->mapFromScene(touchPoint);
        //qDebug() << "WPoint: " <<  touchPoint << " MappedPoint: "<< mappedPoint;

        touchPoint.setX(mappedPoint.x());
        touchPoint.setY(mappedPoint.y());

        QGraphicsSceneMouseEvent event4(QEvent::GraphicsSceneMouseRelease);
        event4.setScenePos(QPoint(x, y));
        event4.setPos(mappedPoint);
        event4.setScreenPos(touchPoint);
        event4.setButton(Qt::LeftButton);
        event4.setButtons(Qt::LeftButton);

        bool x = scene->sendEvent(eventItem,&event4);
        if(x) return;
    }

    qDebug() << "Draw Green circle at: " << x << " " << y << "\n";
    itemptr->setBrush(Qt::green);
    scene->addItem(itemptr);
}

void MainView::blob(cv::Point2i point)
{
    touchEvent(point.x,point.y);
}


void MainView::viewItem(QGraphicsItem* item){
    qDebug() <<"view a item";
    qDebug()<< "pos before view: " <<item->pos();
    qDebug()<< "boundingRect" << item->boundingRect();
    scene->addItem(item);
    qDebug() << "visible" << item->isVisible();
}

void MainView::transform(QTransform t)
{
    //HOW TO:
    // add a Widget
    /* AnalogClock *clock = new AnalogClock();
     QGraphicsProxyWidget*  proxy = scene.addWidget(clock);
     proxy->setTransform(transformation);
     proxy->setPos(100,100);*/

    transformation = t;

    PictureFrameWidget *picWidget = new PictureFrameWidget();
    QGraphicsProxyWidget* proxy = scene->addWidget(picWidget);
    proxy->setTransform(t);
}

void MainView::receiveEvent(TouchEvent touchEvent)
{
  QEvent event = touchEvent.event;
  cv::Point2i point = touchEvent.pointProjector;
  QPoint touchPoint(point.x,point.y);
  qDebug() << point.x << " " << point.y << "check";
  // set 0 before!
  QGraphicsItem *eventItem =  scene->itemAt(point.x,point.y,QTransform());


  qDebug()<< eventItem;
    //NO item there --> Widget chooser!
  if( event.type() == QEvent::GraphicsSceneMouseDoubleClick){
       qDebug() << "item: " <<eventItem;

      QTransform trans = transformer->getUiTransformation(touchEvent.pointPSCam.x,touchEvent.pointPSCam.y);

      WidgetLoader *wl = new WidgetLoader(trans);
      QGraphicsProxyWidget* wLproxy = scene->addWidget(wl);
      wLproxy->setTransform(trans);
      QObject::connect(wl,SIGNAL(setWidget(QString,QTransform)),this,SLOT(setWidget(QString,QTransform)));

}

    if(eventItem!=0){
        QPointF mappedPoint = eventItem->mapFromScene(touchPoint);
        //qDebug() << "WPoint: " <<  touchPoint << " MappedPoint: "<< mappedPoint;

        touchPoint.setX(mappedPoint.x());
        touchPoint.setY(mappedPoint.y());

        QGraphicsSceneMouseEvent event4(event.type());
        event4.setScenePos(QPoint(point.x, point.y));
        event4.setPos(mappedPoint);
        event4.setScreenPos(touchPoint);
        event4.setButton(Qt::LeftButton);
        event4.setButtons(Qt::LeftButton);

        bool x = scene->sendEvent(eventItem,&event4);
        if(x){
            qDebug("SKIPP");
            return;
          }
        else{
            QGraphicsEllipseItem* itemptr = new QGraphicsEllipseItem(point.x,point.y,15,15,0);
            qDebug() << "Draw Green circle at: " << point.x << " " << point.y << "\n";
            itemptr->setBrush(Qt::green);
            scene->addItem(itemptr);
        }
    }





    new TouchItem(point,scene);
}

void MainView::setWidget(QString widget, QTransform transformation)
{
  qDebug()<< "set " << widget << " to " << transformation ;
  //TODO: safe to xml!
  if(widget.operator ==( QString("clock"))){
      qDebug("SET CLOCK");
      AnalogClock *clock = new AnalogClock;
      scene->addItem(clock);
      clock->setTransform(transformation);
      //QGraphicsProxyWidget* proxy = scene->addWidget(clock);
      //proxy->setTransform(transformation);
  }

  if(widget.operator ==( QString("frame"))){
    qDebug("SET FRAME");
    PictureFrameWidget *pfw = new PictureFrameWidget;
    QGraphicsProxyWidget* proxy = scene->addWidget(pfw);
    proxy->setTransform(transformation);
  }

//  if(widget.operator ==( QString("gl"))){
//    qDebug("SET gl");
//    GLWidget *gl = new GLWidget;
//    QGraphicsProxyWidget* proxy = scene->addWidget(gl);
//    proxy->setTransform(transformation);
//  }
//  Add QML Widget
  if(widget.operator ==( QString("bustimetable"))){
      QDeclarativeView *qmlView = new QDeclarativeView;
      qmlView->setSource(QUrl::fromLocalFile("qmlWidget/bustimetable/QMLBusTimetable.qml"));
      //qmlView->setTransform(transformation);
      QGraphicsProxyWidget* proxy = scene->addWidget(qmlView);
      proxy->setTransform(transformation);
  }

  if(widget.operator ==( QString("imagebrowser"))){
      QDeclarativeView *qmlView = new QDeclarativeView;
      qmlView->setSource(QUrl::fromLocalFile("qmlWidget/photoviewer/photoviewer.qml"));
      QGraphicsProxyWidget* proxy = scene->addWidget(qmlView);
      proxy->setTransform(transformation);
  }



//   Add simple website
//  if(widget.operator ==( QString("bustimetable"))){
//    qDebug("SET bustimetable");
//    PanWebViewWidget *webView = new PanWebViewWidget;
//    webView->setUrl(QUrl("http://h.fs-et.de/1240"));
//    webView->setGeometry(0,0,200,200);
//    QGraphicsProxyWidget* proxy = scene->addWidget(webView);
//    proxy->setTransform(transformation);
//  }



}

