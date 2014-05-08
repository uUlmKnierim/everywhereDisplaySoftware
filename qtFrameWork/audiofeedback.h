#ifndef AUDIOFEEDBACK_H
#define AUDIOFEEDBACK_H

#include <QObject>
#include <QSound>
#include <QEvent>
#include <QDebug>
#include <QList>
#include "opencv2/opencv.hpp"

#include <phonon>

class AudioFeedback : public QObject
{
    Q_OBJECT
public:
    explicit AudioFeedback(QObject *parent = 0);

private:
    QSound* mouseClick;

signals:

public slots:
    void mouseEvent(cv::Point2i point, QEvent event);


};

#endif // AUDIOFEEDBACK_H
