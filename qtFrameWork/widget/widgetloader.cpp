#include "widgetloader.h"
#include <QVariant>
#include "analogclock.h"

WidgetLoader::WidgetLoader(QTransform transform, QWidget *parent) :
    QWidget(parent)
{
  this->transform = transform;
  //this->point = point;

  widgetNames.append("Clock");
  widgetIDs.append("clock");
  widgetNames.append("Image Frame");
  widgetIDs.append("frame");
  widgetNames.append("Bus timetable");
  widgetIDs.append("bustimetable");
  widgetNames.append("Image browser");
  widgetIDs.append("imagebrowser");

//  widgetNames.append("");
//  widgetIDs.append("");





  QFont sansFont("Helvetica [Cronyx]", 20);
  QListWidget *listWidget = new QListWidget(this);
  QListWidgetItem *newItem;

  for(int i=0; i<widgetNames.size(); i++){
    newItem = new QListWidgetItem;
    newItem->setText(widgetNames.at(i));
    //Set special widget id
    //TODO: make fully dynamic.
    newItem->setData(5,QVariant(widgetIDs.at(i)));
    newItem->setFont(sansFont);
    QIcon icon;// = new QIcon;
    //ToDo: load correct image
    icon.addPixmap(QPixmap("/home/linaro/qtBiquitousUI/images/c.jpg"));
    newItem->setIcon(icon);
    listWidget->insertItem(i, newItem);
  }
  listWidget->show();

  QObject::connect(listWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(itemClicked(QListWidgetItem*)));
}

void WidgetLoader::paintEvent(QPaintEvent *)
{

}

void WidgetLoader::mousePressEvent(QMouseEvent *event)
{

}

void WidgetLoader::mouseReleaseEvent(QMouseEvent *event)
{

}

void WidgetLoader::mouseDoubleClickEvent(QMouseEvent *event)
{
  qDebug("double/long click");
}

void WidgetLoader::itemClicked(QListWidgetItem *item)
{
  QString itemID = (item->data(5)).toString();
//  qDebug() << "id: "  << itemID;
//  qDebug()<< "Global: " << this->mapToGlobal(this->pos());
//  qDebug()<< "parent: " << this->mapToParent(this->pos());
//  qDebug()<< "view " << item->text() << " now at " << this->pos() << " x : "<< this->x() ;
  emit setWidget(itemID,transform);
  emit deleteLater();
}
