#include <dhq.h>
#include <text.h>
//above no use any more
#include <SoftwareSerial.h>
SoftwareSerial mySerial(4,5);
int dustPin=0;
int mq135Pin=1;
float dustVal=0;
 
int ledPower=2;
int delayTime=280;
int delayTime2=40;
float offTime=9680;
void setup(){
Serial.begin(9600);
mySerial.begin(9600);
pinMode(ledPower,OUTPUT);
pinMode(dustPin, INPUT);
pinMode(mq135Pin,INPUT);
}
 
void loop(){
// ledPower is any digital pin on the arduino connected to Pin 3 on the sensor
digitalWrite(ledPower,LOW); 
delayMicroseconds(delayTime);
dustVal=analogRead(dustPin); 
delayMicroseconds(delayTime2);
digitalWrite(ledPower,HIGH); 
delayMicroseconds(offTime);
String pm2d5="{\"pm2d5\": \"null\"}";
delay(1000);
if(dustVal>36.455){
  float Vo135=analogRead(mq135Pin);
Serial.println((float(dustVal/1024)-0.0356)*120000*0.035/4);
Serial.println(0.6*1000*(Vo135/1024)*(5000/4000));

mySerial.print("{\"pm2d5\":\"");
mySerial.print((float(dustVal/1024)-0.0356)*120000*0.035/4);
mySerial.print("\",\"mq135\":\"");
mySerial.print(0.6*1000*(Vo135/1024)*(5000/4000));
mySerial.println("\"}");
}
}
