#include "tracker.h"

Tracker::Tracker(QObject *parent) :
    QObject(parent)
{
    max_radius = MAXRADIUS;
    max_radius_longpress = LONGPRESSDISTANCE;
}

void Tracker::update(std::vector<TouchEvent> events_new)
{    
    // clear the previous events
    events_previous.clear();

    //copy alive events to previous
    for (int i = 0; i < events.size(); i++){
            if (events[i].lifes > 0){
                events[i].tracked = false;
                events_previous.push_back(events[i]);
            }

            if(events[i].lifes == 0 && events[i].age > 1){
                qDebug("SEND TOUCH EVENT !!! mouse RELEASE");
                qDebug()<< "AT: " << events[i].pointProjector.x << "," << events[i].pointProjector.y;
                events[i].event = QEvent::GraphicsSceneMouseRelease;
                emit sendEvent(events[i]);
            }
    }

    // populate the blobs vector with the current frame
    events.clear();
    events = events_new;

  for (int i = 0; i < events.size(); i++) {
    for (int j = 0; j < events_previous.size(); j++) {
      if (events_previous[j].tracked) continue;
      float dist = sqrt(pow(events[i].pointProjector.x - events_previous[j].pointProjector.x, 2.0) + pow(events[i].pointProjector.y - events_previous[j].pointProjector.y, 2.0));
      if (dist < max_radius) {
          events_previous[j].tracked = true;
          //blobs[i].event = BLOB_MOVE;
          //increase Age
          events[i].age =  events_previous[j].age + 1;
          //copy TimerInfo
          events[i].timer = events_previous[j].timer;
          events[i].timerSet = events_previous[j].timerSet;
          events[i].lpEmit = events_previous[j].lpEmit;
          if(events[i].age==2){
              qDebug("SEND TOUCH EVENT !!! mouse DOWN");
              qDebug()<< "AT: " << events[i].pointProjector.x << "," << events[i].pointProjector.y;
              events[i].event = QEvent::GraphicsSceneMousePress;
              emit sendEvent(events[i]);
          }
          if(events[i].age>3){
              qDebug("SEND TOUCH EVENT !!! mouse MOVE");
              qDebug()<< "AT: " << events[i].pointProjector.x << "," << events[i].pointProjector.y;
               events[i].event =QEvent::GraphicsSceneMouseMove;
              emit sendEvent(events[i]);
          }
      if(dist<max_radius_longpress){
        if(!events[i].timerSet){
            events[i].timer.restart();
            events[i].timerSet = true;
        }
        if(events[i].timer.elapsed()>LONGPRESS && !events[i].lpEmit){
            events[i].lpEmit = true;
            qDebug("SEND TOUCH EVENT !!! mouse LongPRESS(doubleClick)");
            qDebug()<< "AT: " << events[i].pointProjector.x << "," << events[i].pointProjector.y;
            events[i].event = QEvent::GraphicsSceneMouseDoubleClick;
            emit sendEvent(events[i]);
        }
      }
      else{
          events[i].lpEmit = false;
          events[i].timerSet = false;
        }
      }
    }
  }

    for (int i = 0; i < events_previous.size(); i++) {
        if (!events_previous[i].tracked) {
            events_previous[i].lifes--;
            events.push_back(events_previous[i]);
        }
    }
}
