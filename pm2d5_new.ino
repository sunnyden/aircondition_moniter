#include <Wire.h>

#include <MS5611.h>

#include <DHT_new.h>
#define DHTPIN 7 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
//#include <DHT22.h>
#include <stdio.h>
MS5611 ms5611;
double referencePressure;
float baseTemp=28.7;//标准温度 old:25.9
float noDust=944;//标准电压 old:1050
int ledPower_2d5=5;  //PM2.5传感器的led灯数字引脚。
int pm25data=10;//pm2.5模拟输入
// Data wire is plugged into port 7 on the Arduino
// Connect a 4.7K resistor between VCC and the data pin (strong pullup)
#define DHT11_PIN 7
// Setup a DHT22 instance
float air_Temp;

/*
LCD  Arduino
PIN1 = GND
PIN2 = 5V
RS(CS) = 8; 
RW(SID)= 9; 
EN(CLK) = 3;
PIN15 PSB = GND;
*/
 #include "dhq.h"
#include "LCD12864RSPI.h"
#define AR_SIZE( a ) sizeof( a ) / sizeof( a[0] )
 int dustPin=0;

float dustVal=0;

int ledPower=2;
int delayTime=280;
int delayTime2=40;
float offTime=9680;
char title0[]={
  0xBF, 0xD5,
  0xC6, 0xF8,
  0xD6, 0xCA,
  0xC1, 0xBF,
    0xBC, 0xE0,
  0xB2, 0xE2
     };                    //移动式空气质量
int count_send=0;
char title1[]={

  0xD3, 0xEB,
  0xCA, 0xB5,
  0xCA, 0xB1,
  0xB7, 0xA2,
  0xB2, 0xBC,
  0xCF, 0xB5,
  0xCD, 0xB3
     };                    //监测与实时发布系统
char title2[]="Starting UP";

void checkSettings()
{
  Serial.print("Oversampling: ");
  Serial.println(ms5611.getOversampling());
}
 
void setup()
{
    Serial.begin(115200);
   Serial3.begin(115200);

pinMode(pm25data, INPUT);
pinMode(ledPower,OUTPUT);
pinMode(dustPin, INPUT);
pinMode(ledPower_2d5,OUTPUT);
LCDA.Initialise(); // 屏幕初始化
delay(100);
LCDA.CLEAR();//清屏
delay(100);
LCDA.DisplayString(0,0,title0,AR_SIZE(title0));//第一行第三格开始，显示文字极客工坊
delay(100);
LCDA.DisplayString(1,1,title1,AR_SIZE(title1));//第三行第二格开始，显示文字geek-workshop
delay(100);
LCDA.DisplayString(3,1,title2,AR_SIZE(title2));

  // Get reference pressure for relative altitude
  
    while(!ms5611.begin())
  {
    Serial.println("Could not find a valid MS5611 sensor, check wiring!");
    delay(500);
  }
referencePressure = ms5611.readPressure();
  // Check settings
  checkSettings();
delay(3000);
LCDA.CLEAR();//清屏
delay(100);
}
 char* itostr(char *str, int i) 
{
    sprintf(str, "%d", i);
    return str;
}
char keliwu[]={
  0xBF, 0xC5,
  0xC1, 0xA3,
  0xCE, 0xEF,
  0x3A, 0x00
     };                    //颗粒物:
     
float getpm25(){

float Vo;
float Ao;
float dlt_Mv;
float Sum=0;
int count=0;
float pm25;
do{

digitalWrite(ledPower_2d5,HIGH);
delayMicroseconds(280);
Ao=analogRead(pm25data);
delayMicroseconds(40);
digitalWrite(ledPower_2d5,LOW);
delayMicroseconds(9680);
count=count+1;
Sum=Sum+Ao;
}while(count<=100);
float averAo=Sum/count;
String strAo=floatToString("",averAo,1);
//myGLCD.print(strAo, LEFT, 125);
Vo=(averAo/1024)*5000.0;
dlt_Mv=Vo-get_Vs();
pm25=0.6*dlt_Mv;
return pm25;
Sum=0;
count=0;
}
float get_Vs() //此处是修正基准电压 Vs 漂移的。
//获得基准电压
{
  float Vs;
if(air_Temp < 40 && air_Temp > -10)//比较温度大小，分段函数
{
float dlt_Temp= baseTemp-air_Temp;
Vs=noDust-6*dlt_Temp;
}
else if( air_Temp>=40 )
{
float dlt_Temp= baseTemp-40;
Vs=noDust-((dlt_Temp)*6-(air_Temp-40)*1.5);
}
return Vs;
}

void loop()
{
 // Serial.println("LOOPBEGIN");
Serial.println("Requesting data...");
  float pm2d5 =getpm25();
digitalWrite(ledPower,LOW); 
delayMicroseconds(delayTime);
dustVal=analogRead(dustPin); 
delayMicroseconds(delayTime2);
digitalWrite(ledPower,HIGH); 
delayMicroseconds(offTime);
delay(1000);int dust=0;
if (dustVal>36.455)
dust=int((float(dustVal/1024)-0.0356)*120000*0.035);
//dust=438438;
LCDA.DisplayString(1,0,keliwu,AR_SIZE(keliwu));
String strdust;
String blank;
blank ="";
if(dust>4){strdust=floatToString("",dust,0);}//Serial.println(strdust);
String length0=strdust;

LCDA.DisplayString(1,4,strdust+"  ",length0.length()+2);

String strtemp;
String strwet;
String txt;
//char* chtxt;
//delay(20);

  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit
  float f = dht.readTemperature(true);
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
 float hi = dht.computeHeatIndex(f, h);
air_Temp=t;
      strtemp=floatToString("",t,0);
      strwet=floatToString("",h,0);
      txt=strtemp+strwet;
      //txt=chtxt;
      char chntemp[]={  0xCE, 0xC2,
  0xB6, 0xC8};
    LCDA.DisplayString(3,0,chntemp,AR_SIZE(chntemp));
      LCDA.DisplayString(3,2,":"+strtemp+"C",strtemp.length()+2);
          char chnwet[]={  0xCA, 0xAA,
  0xB6, 0xC8};
    LCDA.DisplayString(3,4,chnwet,AR_SIZE(chntemp));
      LCDA.DisplayString(3,6,":"+strwet+"%",strtemp.length()+2);
float  realPressure = ms5611.readPressure()/100;
String strPressure = floatToString("",realPressure,0);
char chnPressure[] =  {0xC6, 0xF8,
  0xD1, 0xB9};
   LCDA.DisplayString(2,0,chnPressure,AR_SIZE(chnPressure));
   LCDA.DisplayString(2,2,":"+strPressure+"hPa",strPressure.length()+3+1);
String str2d5= floatToString("",pm2d5,0);
   LCDA.DisplayString(0,0,"PM2.5:"+str2d5+"ug/m3 ",str2d5.length()+12);
 count_send=count_send+1;
Serial.print(count_send);
if(count_send==14){
http(str2d5,strtemp,strwet,strdust,strPressure);
count_send=0;
}
}
void http(String post2d5,String temp,String wet,String keliwu,String pressure){
String http_request;
http_request = "GET /env/input.php?";
http_request += "dev_id=02&pass=m62078&";
http_request += "pm2d5="+post2d5+"&";
http_request += "temperature="+temp+"&";
http_request += "pressure="+pressure+"&";
http_request += "humidity="+wet+"&";
http_request += "particles="+keliwu;
int len = http_request.length()+1;  
  Serial3.println("AT+RST");
 delay(5000);

 delay(100);
 Serial3.println("AT+CIPSTART=\"TCP\",\"58.63.232.138\",62078");
 delay(500);

 delay(500);
  Serial3.print("AT+CIPSEND=");
  Serial3.println(len);
 delay(300);

  delay(1000);
 Serial3.println(http_request); 
   delay(1000);

    delay(1000);
   Serial3.print("AT+CIPCLOSE"); 
//Serial3.flush();
   delay(1000);

 }
