#ifndef WIDGETLOADER_H
#define WIDGETLOADER_H

#include <QWidget>
#include <QPushButton>
#include <QtDebug>
#include <QListWidget>
#include <QIcon>
#include <QStringList>

class WidgetLoader : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetLoader(QTransform transform, QWidget *parent = 0);
    void paintEvent(QPaintEvent *);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

private:
    QStringList widgetNames;
    QStringList widgetIDs;
    QTransform transform;


signals:
    void setWidget(QString widget, QTransform transform);

public slots:
  void itemClicked ( QListWidgetItem * item );

};

#endif
