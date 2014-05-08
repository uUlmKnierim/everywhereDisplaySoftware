#ifndef TRACKER_H
#define TRACKER_H

#include <QObject>

#include "touchevent.h"
#include <stdlib.h>
#include <stdio.h>
#include "opencv2/opencv.hpp"
#include <QEvent>
#include <QDebug>

#include "settings.h"

class Tracker : public QObject
{
    Q_OBJECT
public:
    explicit Tracker(QObject *parent = 0);

signals:
    void sendEvent(TouchEvent event);

public slots:
    void update(std::vector<TouchEvent> events_new);

private:
    int max_radius;
    int max_radius_longpress;
    std::vector<TouchEvent> events, events_previous;

};

#endif // TRACKER_H




