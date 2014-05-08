#include "audiofeedback.h"

//#include <Phonon/AudioOutputDevice>

AudioFeedback::AudioFeedback(QObject *parent) :
    QObject(parent)
{
//    mouseClick = new QSound("/home/linaro/qtBiquitousUI/sounds/click.wav");


     QList<Phonon::AudioOutputDevice> devs = Phonon::BackendCapabilities::availableAudioOutputDevices();

     if(devs.size() ==0)
         qDebug("no devices");
     else
         qDebug("some Devices ;)");
     qDebug()<< "there are: "  << devs.size() << "devices";

    for(int i=0; i< devs.size(); i++){
    Phonon::AudioOutputDevice device =  devs.at(i);
    qDebug()<< "device: " << device.name() << "\nDescription: "<< device.description();
    }

    //mediaObject = new Phonon::MediaObject();
   // mediaObject->setCurrentSource(Phonon::MediaSource("/Users/pascalknierim/fussscreen/click.wav"));
   // audioOutput = new Phonon::AudioOutput(Phonon::NoCategory);
    //audioOutput->
   // Phonon::Path path = Phonon::createPath(mediaObject, audioOutput);
    //mediaObject->play();



}

void AudioFeedback::mouseEvent(cv::Point2i point, QEvent event)
{
    if(event.type() == QEvent::GraphicsSceneMousePress){
            qDebug("Play Mouse klick");
//            qDebug()<< "Sound: " << mouseClick->fileName();
//            mouseClick->play();
//            qDebug()<< "sound: "<< QSound::isAvailable();
//            mediaObject->play();

    }

    if(event.type() == QEvent::GraphicsSceneMouseRelease){
            qDebug("Play Mouse klick");
            //mouseClick->play();
    }
}
