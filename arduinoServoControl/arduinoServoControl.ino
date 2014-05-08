#include <Servo.h>
#include <Wire.h>
#include <LSM303.h>

#define NOAUTOFOCUS  // undefine for autofokus

//Commands:
#define UP 04
#define DOWN 06
#define LEFT 05
#define RIGHT 07
#define SENSORDATA 00

#define ROT 1  // Pin 11 Color Yellow 
#define TILT 2 // Pin 10 Color Grey 
#define FOCUS 3
#define FOCUSPIN 2

#define ROTNEUTRAL 1480
#define NEUTRAL 1500
#define FOCUSNEUTRAL 1200
#define SPEED 10
#define SAMPLES 150

// security border
#define ROTMIN 1360
#define ROTMAX 1600

#define TILTMIN 1475
#define TILTMAX 1640


Servo rot;
Servo tilt;
Servo focus;

LSM303 compass;
char report[80];
float alphaMagnet = 0.01;
float alphaAccelero = 0.025;


int command, pos;
float accelerometer = 0;

int rotPos = ROTNEUTRAL;
float rotAccPos = NEUTRAL;

int tiltPos = 2000;
float tiltAimPos = NEUTRAL+20;
float tiltAccPos = NEUTRAL+20;

int focusPos = 1200;
int focusAccPos = 1200;

int count = 0;
boolean done = false;
boolean accDone = false;
boolean printDone = false; 

long previousMillis = 0;
long interval = 100;

void setup() {
#ifndef NOAUTOFOCUS
  focus.attach(3);
#endif
  rot.attach(11);
  tilt.attach(10);
  Serial.begin(9600);
  
  Wire.begin();
  compass.init();
  compass.enableDefault();
  compass.read();
  accelerometer = compass.a.x;
  
  rot.writeMicroseconds(NEUTRAL);
  Serial.println("Setup Done");
  
}

static inline int8_t sgn(int val) {
  if (val < 0) return -1;
  if (val==0) return 0;
  return 1;
}


void loop() {
  
  if(!accDone){
    compass.read();
    count++;
    //Digital Low Pass for Noise Reduction for Magnetic Sensor
   accelerometer = accelerometer + alphaAccelero * (compass.a.x - accelerometer);
   //Serial.println(accelerometer);
  }
  
  if (Serial.available()>1) {
      delay(10);  //delay to allow buffer to fill 
      command  = Serial.parseInt();
      
      if(command == SENSORDATA){
        // get x SensorSamples an print them;      
        compass.read();
        float sampleAccelerometer;
        sampleAccelerometer = compass.a.x;
        for(int i=0; i< SAMPLES; i++){
          compass.read();
          sampleAccelerometer = sampleAccelerometer + 0.025 * (compass.a.x - sampleAccelerometer);
      }
        Serial.print("position#");
        Serial.print(rotAccPos);     
        Serial.print("#");
        Serial.println(sampleAccelerometer);
        return;
      }
      
      if(command  == UP){
        tiltAimPos = tiltAimPos -4;
        if(tiltAimPos<TILTMIN){
          tiltAimPos = TILTMIN;
        }
        return;
      }
      
      if(command  == DOWN){
        tiltAimPos = tiltAimPos +4;
        if(tiltAimPos>TILTMAX){
          tiltAimPos = TILTMAX;
        }
        return;
      }
      
      if(command  == LEFT){
        rotPos = rotPos + 4;
        if(rotPos > 1600)
          rotPos = 1360;
        return;
      }
      
      if(command  == RIGHT){
        rotPos = rotPos - 4;
        if(rotPos< 1360){
          rotPos = 1600;
        }
        return;
      }      
      
      delay(10);
      pos = Serial.parseInt();

//      Serial.print("Servo: " );
//      Serial.println(command );
//      Serial.print("Pos: ");
//      Serial.println(pos);

    if(command == ROT){
        printDone = true;
        rotPos = pos;
    }
  
    if(command == TILT){
      printDone = true;
      tiltPos = pos;
      accDone = false;
      return;
    }
  
    if(command  == FOCUS){
      focusPos =  pos;
    } 
  }
  
  
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis > interval) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;
 if(count>0){   
   Serial.print("count: ");
   Serial.println(count);
 }
   if(rotPos>1600 || rotPos < 1360){
     Serial.println("Wrong Rotation input");
     return;
   }
   

   
   if(rotPos!=rotAccPos){
     int tmp = rotPos-rotAccPos;
   if(abs(tmp)<SPEED){
       rotAccPos = rotPos;
   }else{
     tmp = sgn(tmp);
     rotAccPos = rotAccPos + (tmp*SPEED);
   }
   if(rotAccPos>ROTMIN && rotAccPos<ROTMAX){
     rot.writeMicroseconds(rotAccPos);
   }
   else{
     Serial.println("Stop rotation by security check");
   }
  }

  
  float diff = accelerometer - tiltPos;
  if(abs(diff)<250 && !accDone && count>50) {
    accDone = true;
  }
    
  if(abs(diff)>=300 && !accDone){
    float c = abs(diff)*0.0005;
    if(c>4) c=4;
//        Serial.print("Value: ");
//        Serial.println(accelerometer);
//        Serial.print("Diff:");
//        Serial.println(diff);
//        Serial.print("\t");
//        Serial.println(c);
    
    if(diff>0)
    tiltAimPos = tiltAccPos + c;
    else
    tiltAimPos = tiltAccPos - c;
  }

  if(tiltAimPos!=tiltAccPos){
    int tmp = tiltAimPos-tiltAccPos;
    if(abs(tmp)<SPEED){
    tiltAccPos = tiltAimPos;
   }else{
    tmp = sgn(tmp);
    tiltAccPos = tiltAccPos + (tmp*SPEED);
   }
    
   if(tiltAccPos<TILTMAX && tiltAccPos >TILTMIN){
    tilt.writeMicroseconds(tiltAccPos);
   }else{
     Serial.println("Error: Tilt Value not in range");
   }
  }
  
#ifndef NOAUTOFOCUS
  if(focusPos == focusAccPos && focus.attached()){
    focus.writeMicroseconds(focusAccPos+5);
    delay(200);
    focus.writeMicroseconds(focusAccPos-5);
    delay(200);
    focus.writeMicroseconds(focusAccPos);
    delay(200);
    focus.detach();
    pinMode(3, OUTPUT);
    digitalWrite(3, LOW);
    Serial.println("LOG: Focus Servo Detached");
  }
  if(focusPos!=focusAccPos){
  if(!focus.attached())
      focus.attach(3);
    int tmp = focusPos-focusAccPos;
    if(abs(tmp)<SPEED*2){
    focusAccPos = focusPos;
   }else{
    tmp = sgn(tmp);
    focusAccPos = focusAccPos + (tmp*SPEED*2);
   }
    focus.writeMicroseconds(focusAccPos);
  }
#endif
  
  if((rotPos==rotAccPos)&&accDone && printDone ){
    delay(500);
    Serial.println("MovementDone");
    printDone = false;
  }
  count = 0;
}
}

