#ifndef PICTUREFRAMEWIDGET_H
#define PICTUREFRAMEWIDGET_H

#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QDebug>
#include <QPoint>
#include <QPixmap>
#include <QStringList>
#include <QMouseEvent>


class PictureFrameWidget : public QWidget
{
public:
    PictureFrameWidget(QWidget *parent = 0);
    void paintEvent(QPaintEvent *);

private:
    QStringList images;
    int imageNr;
    QPoint oldPos;
    bool moving;

protected:
    //void mousePressEvent(QGraphicsSceneMouseEvent *event);
    //void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *evt);
    void mouseDoubleClickEvent(QMouseEvent *evt);
};

#endif // PICTUREFRAMEWIDGET_H
