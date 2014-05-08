#ifndef SERVER_H
#define SERVER_H

#include <QtNetwork>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QString>
#include <QStringList>
#include "domparser.h"

class Server: public QObject
{
Q_OBJECT
public:
  Server( DomParser *_parser);
  ~Server();

signals:
  void moving();
  void save(QString name ,int rotation, int tilt );
  float getDistance(int x, int y);
  void disconnected();
  void calibrate();

public slots:
  void acceptConnection();
  void startRead();
  void close();

private:
  QTcpServer server;
  QTcpSocket* client;
  DomParser* parser;

  void setFocus(float distance);
};

#endif // SERVER_H
