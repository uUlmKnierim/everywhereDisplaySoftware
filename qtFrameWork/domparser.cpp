#include "domparser.h"

#include <QTextStream>
#include <QStringList>
#include <QtDebug>

DomParser::DomParser(QObject* parent): QObject(parent)
{


    doc = QDomDocument("qtBiquitousUI");
    QFile file("./test.xml");
    if (!file.open(QIODevice::ReadOnly)){
        QDomElement root = doc.createElement("qtBiquitousUI");
        doc.appendChild(root);
        save();
        return;
    }
    if (!doc.setContent(&file)) {
        QDomElement root = doc.createElement("qtBiquitousUI");
        doc.appendChild(root);
        save();
        file.close();
        return;
    }
    file.close();
}

QStringList DomParser::surfaceNames()
{
    QStringList names;
    QDomNode root = doc.firstChild();
    QDomNodeList nodes = root.childNodes();

    for(int i=0; i< nodes.size(); i++){
    QDomNode node = nodes.at(i);
    QDomElement element = node.toElement();
    if(node.nodeName()==QString("Surface")){
        QString name = element.attribute("name", "none");
        std::cout << name.toUtf8().constData() << " name!!!"<< std::endl;
        names.append(name); // .toUtf8().constData()
    }
    }
    return names;
}

void DomParser::getTiltPan(QString surface, int *tilt, int *pan)
{
    QDomNode root = doc.firstChild();
    QDomNodeList nodes = root.childNodes();

    for(int i=0; i< nodes.size(); i++){
    QDomNode node = nodes.at(i);
    QDomElement element = node.toElement();
    if(node.nodeName()==QString("Surface")){
        QString name = element.attribute("name", "none");
        if(name == surface ){
            (*tilt)=element.attribute("tilt","0").toInt(NULL,10);
            (*pan) = element.attribute("pan","0").toInt(NULL,10);
            return;
      }
    }
    }
}

void DomParser::delteSurface(QString surface)
{
 qDebug() << "delete " << surface  << "in parser" ;

 QDomNode root = doc.firstChild();
 QDomNodeList nodes = root.childNodes();
 for(int i=0; i< nodes.size(); i++){
 QDomNode node = nodes.at(i);
 QDomElement element = node.toElement();
 if(node.nodeName()==QString("Surface")){
     QString name = element.attribute("name", "none");
     if(name == surface ){
      root.removeChild(node);
      qDebug("Node removed");
      save();
      return;
   }
 }
 }



}



void DomParser::save()
{
    //Debug
    std::cout << "Dom File:" << std::endl;
    QString xml = doc.toString();
    std::cout << xml.toUtf8().constData() << std::endl;

    QFile File("./test.xml");
       if ( File.open(QIODevice::WriteOnly) )
       {
          QTextStream TextStream(&File);
          TextStream << doc.toString() ;
          File.close();
       }
       else
          qWarning("danger :(");
}

void DomParser::saveNewSurface(QString name, int rotation, int tilt)
{
    std::cout << "SLOT CALLED saveNewSurface" << std::endl;
    QDomElement surface = doc.createElement("Surface");
    doc.firstChildElement("qtBiquitousUI").appendChild(surface);
    surface.setAttribute("name",name);
    surface.setAttribute("pan", rotation);
    surface.setAttribute("tilt", tilt);
    save();
}


