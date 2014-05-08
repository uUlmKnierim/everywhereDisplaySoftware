#include "server.h"
#include <iostream>

#include <SerialStream.h>
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <string>
#include "mainview.h"

#define ARDUINO "/dev/ttyUSB0"
#define ROT 1
#define TILT 2
#define FOCUS 3

//Super important
//stty -F /dev/ttyUSB0 cs8 9600 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke noflsh -ixon -crtscts

using namespace std;
using namespace LibSerial;

SerialStream serial_port ;

Server::Server(DomParser* _parser): QObject(0)
{
  parser = _parser;
  connect(&server, SIGNAL(newConnection()), this, SLOT(acceptConnection()));

  serial_port.Open(ARDUINO) ;
     if ( ! serial_port.good() )
     {
         std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] "
                   << "Error: Could not open serial port."
                   << std::endl ;
         exit(1) ;
     }
     //
     // Set the baud rate of the serial port.
     //
     serial_port.SetBaudRate( SerialStreamBuf::BAUD_9600 ) ;
     if ( ! serial_port.good() )
     {
         std::cerr << "Error: Could not set the baud rate." <<
std::endl ;
         exit(1) ;
     }
     //
     // Set the number of data bits.
     //
     serial_port.SetCharSize( SerialStreamBuf::CHAR_SIZE_8 ) ;
     if ( ! serial_port.good() )
     {
         std::cerr << "Error: Could not set the character size." <<
std::endl ;
         exit(1) ;
     }
     //
     // Disable parity.
     //
     serial_port.SetParity( SerialStreamBuf::PARITY_NONE ) ;
     if ( ! serial_port.good() )
     {
         std::cerr << "Error: Could not disable the parity." <<
std::endl ;
         exit(1) ;
     }
     //
     // Set the number of stop bits.
     //
     serial_port.SetNumOfStopBits( 1 ) ;
     if ( ! serial_port.good() )
     {
         std::cerr << "Error: Could not set the number of stop bits."
                   << std::endl ;
         exit(1) ;
     }
     //
     // Turn off hardware flow control.
     //
     serial_port.SetFlowControl( SerialStreamBuf::FLOW_CONTROL_NONE ) ;
     if ( ! serial_port.good() )
     {
         std::cerr << "Error: Could not use hardware flow control."
                   << std::endl ;
         exit(1) ;
     }

  server.listen(QHostAddress::Any, 8888);
}

Server::~Server()
{
  server.close();
}

void Server::acceptConnection()
{
  std::cout << "New Connection" << std::endl;
  client = server.nextPendingConnection();
  connect(client, SIGNAL(readyRead()),
    this, SLOT(startRead()));

  connect(client,SIGNAL(disconnected()),this,SLOT(close()));
}




void Server::startRead()
{
      char buffer[1024] = {0};
      client->read(buffer, client->bytesAvailable());
      cout << buffer << endl;
      if(buffer[0] == 'l'){
           emit moving();
//           rot = rot + 5;
//           if(rot > 230)
//                   rot=0;
           serial_port <<"05"<<endl;
           //serial_port << ROT << "#" << rot << endl;
      }

      if(buffer[0] == 'r'){
          emit moving();
//          rot = rot - 5;
//                  if(rot < -30)
//                  rot = 210;
          serial_port <<"07"<<endl;
          //serial_port << ROT <<"#" << rot << endl;
      }

      if(buffer[0] == 'u'){
          emit moving();
//          tilt = tilt - 5;
          //TODO: overflow
          serial_port <<"04"<<endl;
          //serial_port << TILT <<"#" << tilt << endl;
      }

      if(buffer[0] == 'd'){
          emit moving();
//          tilt = tilt + 5;
          //TODO: Overflow
          serial_port <<"06"<<endl;
          //serial_port << TILT <<"#" << tilt << endl;
      }

      if(buffer[0] == 's'){
        std::string strBuffer(buffer);
        unsigned pos = strBuffer.find("#");
        std::string name = strBuffer.substr(pos+1);
        std::cout << "Name of surface: " << name << std::endl;
        //Send position request
        serial_port << "00" << endl;

        // Read Position response
        char temp=0;
        int i=0;
        char cstr[64];
        while(temp!='\n')
        {
            try
            {
                serial_port.get(temp);
               // temp=serial_port.ReadByte(100);
            }
            catch(SerialPort::ReadTimeout &e)
            {
                cout<<"Read Timeout"<<endl;
            }
            if((temp!='\n')&&(temp!=0)&&(temp!=' '))
            {
                cstr[i]=temp;
                ++i;
                //cout<<i++<<temp<<'x'<<endl;
            }
        }
        cstr[i]='\0';

        std::string dataBuffer(cstr);
        unsigned pos1 = dataBuffer.find("#");
        unsigned pos2 = dataBuffer.find("#",pos1+1);

        std::string rotString = dataBuffer.substr(pos1+1, pos2-pos1-1);
        std::string tiltString = dataBuffer.substr(pos2+1);

        std::cout <<"i got from " << dataBuffer << "\nrot: " << rotString << " and tilt: " << tiltString << std::endl;


        int rot =  QString::fromStdString(rotString).toFloat(NULL);
        int tilt =  QString::fromStdString(tiltString).toFloat(NULL);

        std::cout << "Rotation: " << rot << " Tilt: "<< tilt << std::endl;

        emit save(QString::fromStdString(name),rot,tilt);
      }

      if(buffer[0] == 'p'){
          //ToDo Distance
          char * lf ="\n" ;
          QStringList top = parser->surfaceNames();
          for ( QStringList::Iterator it = top.begin(); it != top.end(); ++it ) {
                 printf( "%s \n", (*it).toUtf8().constData() );
                 client->write((*it).toUtf8());
                 client->write(lf);
           }
          client->write(lf); // EoS
          client->flush(); //is, like always, important
      }

      if(buffer[0] == 'm'){
          //TODO: Distance
          std::string strBuffer(buffer);
          unsigned pos = strBuffer.find("#");
          std::string surface = strBuffer.substr(pos+1);
          std::cout << "set Projector to: " << surface << std::endl;
          int tilt,pan;
          parser->getTiltPan(QString::fromStdString(surface),&tilt,&pan);

          qDebug() << "set Projector to: "<< QString::fromStdString(surface) <<" Tilt: " << tilt << " Pan: " << pan;
          serial_port << ROT <<"#" << pan << endl;
          serial_port << TILT <<"#" << tilt << endl;
          emit moving();
      }

      if(buffer[0] == 'x'){
          std::string strBuffer(buffer);
          unsigned pos = strBuffer.find("#");
          std::string surface = strBuffer.substr(pos+1);
          std::cout << "delete " << surface << std::endl;
          parser->delteSurface(QString::fromStdString(surface));
      }

      if(buffer[0] == 'c'){
          std::cout << "recalibrate "<< std::endl;
          emit calibrate();
      }

      float distance =  getDistance(320,240);
      std::cout << "after emit distance : " << distance << std::endl;
      setFocus(distance);
}

void Server::close()
{
    std::cout << "close and delte client" << std::endl;

    float distance =  getDistance(320,240);
    std::cout << "after emit distance : " << distance << std::endl;
    setFocus(distance);

    emit disconnected();
    client->close();
}

void Server::setFocus(float distance)
{
    int millis = -34291.91563/distance + 1737.623637; // regression calculation
    std::cout << "Millis to set " << millis << std::endl;
    if(millis<1125 || millis > 1900) return; // Servo can't do this

    //FOCUS IS DISABLED !!!!
    serial_port << FOCUS <<"#"<< millis<< endl;
}
