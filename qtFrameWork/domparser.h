#ifndef DOMPARSER_H
#define DOMPARSER_H

#include <QString>
#include <QDomDocument>
#include <QDomAttr>
#include <QObject>
#include <stdio.h>
#include <iostream>
#include <QFile>

class DomParser: public QObject
{
Q_OBJECT

public:
    DomParser(QObject * parent = 0);
   // ~DomParser();
    QStringList surfaceNames();
    void getTiltPan(QString surface, int* tilt, int* pan);
    void delteSurface(QString surface);

private:
    void save();
    QDomDocument doc;

public slots:
    void saveNewSurface(QString name, int rotation, int tilt );
};

#endif // DOMPARSER_H
