#include<dht.h>
#include <UTFT.h>
#include <dhq.h>
#include <text.h>
#include <chsprint.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <Wire.h>
#include <BH1750FVI.h>
#include <air.h>
BH1750FVI LightSensor;
#define DHT11_PIN 4 //dht11的数字输出引脚
extern uint8_t SmallFont[];
UTFT myGLCD(QD220A,A2,A1,A5,A4,A3); //A1-A5分配给了显示器（还有A0）
int errCount=0;//Error occured count,over 100 reinit.
float baseTemp=28.7;//标准温度 old:25.9
float noDust=944;//标准电压 old:1050
int ledPower=5;  //PM2.5传感器的led灯数字引脚。
int pm25data=10;//pm2.5模拟输入
int mq135Pin=10;//4个气体传感器的链接
float lux=-1;
int mq2Pin=7;
int mq7Pin=8;
int mq9Pin=9;
float air_Temp;
float air_Wet;
float curVo;
String longitute="";
String latitude="";
String height="";
String locat="";
dht DHT;//定义DHT——温湿度传感器
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);//定义压敏、温度传感器

///////////////////////////初始化开始//////////////////////////////
void setup(){

randomSeed(analogRead(0));
myGLCD.InitLCD();
myGLCD.setFont(SmallFont);
delay(100);//等待同步
myGLCD.clrScr();//清空屏幕
//绘图开始
//亮度传感器：
serialinit();
dirtsetup();
  LightSensor.begin();
  /*
 Set the address for this sensor 
 you can use 2 different address
 Device_Address_H "0x5C"
 Device_Address_L "0x23"
 you must connect Addr pin to A3 .
 */
  LightSensor.SetAddress(Device_Address_H);//Address 0x5C
 // To adjust the slave on other address , uncomment this line
 // lightMeter.SetAddress(Device_Address_L); //Address 0x5C
 //-----------------------------------------------
  /*
   set the Working Mode for this sensor 
   Select the following Mode:
    Continuous_H_resolution_Mode
    Continuous_H_resolution_Mode2
    Continuous_L_resolution_Mode
    OneTime_H_resolution_Mode
    OneTime_H_resolution_Mode2
    OneTime_L_resolution_Mode
    
    The data sheet recommanded To use Continuous_H_resolution_Mode
  */

  LightSensor.SetMode(Continuous_H_resolution_Mode);
  
  Serial.println("I/LightSensor Running...");
Serial.print("I/currunt brightness:");
Serial.print(LightSensor.GetLightIntensity());

myGLCD.fillScr(0, 0, 255);
myGLCD.setColor(255, 0, 0);
myGLCD.fillRoundRect(40, 57, 179, 119);  
myGLCD.setColor(255, 255, 255);
myGLCD.setBackColor(255, 0, 0);
//绘图结束
Show_CH_Font16(110,0,12,titlechs,0, 0, 255,255,255,255);
Show_CH_Font16(110,82,5,loadchs,255, 0, 0,255,255,255);//显示欢迎信息
  myGLCD.setColor(0, 255, 0);
  myGLCD.setBackColor(0, 0, 255);
  myGLCD.print("setting pins", CENTER, 146);
//pinMode(pm25data, INPUT);
  getlight();
pinMode(mq135Pin,INPUT); //fixed
  getlight();
pinMode(mq2Pin,INPUT);
pinMode(mq7Pin,INPUT);
pinMode(mq9Pin,INPUT);
myGLCD.print("setting serial", CENTER, 146);
  getlight();
Serial1.begin(115200);
delay(300);
pinMode(ledPower,OUTPUT);
myGLCD.print("begin to load sim908", CENTER, 146);
sim908_gsm_init();
 myGLCD.print("                                       ", CENTER, 146);
myGLCD.print("Setting up GPS...", CENTER, 146);
delay(300);
gpsInit();
 myGLCD.print("                                       ", CENTER, 146);
  myGLCD.print("done loading", CENTER, 146);
float tmp=getPressure(2);
  getlight();
updateWet();
  if(!bmp.begin())
  {
    /* There was a problem detecting the BMP085 ... check your connections */
    myGLCD.print("PressureSensorERROR!", CENTER, 146);
    while(1);
  }

//
myGLCD.clrScr();//清空屏幕
//绘图开始
myGLCD.fillScr(255, 255, 255);
myGLCD.setColor(0, 0, 0);
myGLCD.setColor(0, 0, 0);
myGLCD.setBackColor(255, 255, 255);
Show_CH_Font16(110,0,12,titlechs,0, 0, 0,255,255,255);
}
///////////////////////////初始化结束//////////////////////////////
float sum_pm2d5_net=0;
float sum_mq135=0;
float sum_mq7=0;
float sum_mq2=0;
float sum_mq9=0;
float sum_pressure=0;
float sum_wet=0;
float sum_Temp=0;
float spcount=0;
void loop(){
///单片机周期性工作区
/*
int mq135Pin=6;//4个气体传感器的链接
int mq2Pin=7;
int mq7Pin=8;
int mq9Pin=9;
*/

update_pm2d5();//xinzhidong
updateLocation();
//sim908_http("www.denghaoqing.com/wendu/test.txt");
delay(100);
//loop

while(Serial1.available()>0){
Serial1.read();
}
//float pm2d5 =getpm25(); --past
getlight();
float Vo135=analogRead(mq135Pin);
float Vo7=analogRead(mq7Pin);
float Vo2=analogRead(mq2Pin);
float Vo9=analogRead(mq9Pin);
float mq135=0.6*1000*(Vo135/1024)*(5000/4000);
float mq7=0.6*1000*(Vo7/1024)*(5000/4000)*0.1;
float mq2=0.6*1000*(Vo2/1024)*(5000/4000)*0.1;
float mq9=0.6*1000*(Vo9/1024)*(5000/4000);
float pressure=getPressure(1);
sum_mq135=sum_mq135+mq135;
sum_pm2d5_net=sum_pm2d5_net+cur_pm2d5;//use new
sum_mq7=sum_mq7+mq7;
sum_mq2=sum_mq2+mq2;
sum_mq9=sum_mq9+mq9;
sum_pressure=sum_pressure+pressure;
updateWet();
sum_wet=sum_wet+air_Wet;
sum_Temp=sum_Temp+getPressure(2);
spcount=spcount+1;
String strpm1=floatToString("",cur_pm1,1),strpm10=floatToString("",cur_pm10,1),strpm0d3=floatToString("",cur_pm0d3_pcs,1),strpm1_pcs=floatToString("",cur_pm1_pcs,1),str2d5_pcs=floatToString("",cur_pm2d5_pcs,1),strpm5=floatToString("",cur_pm5_pcs,1),strpm10_pcs=floatToString("",cur_pm5_pcs,1),strlux=floatToString("",lux,1);

if(spcount==10){//!!!
String str135=floatToString("",sum_mq135/spcount,4),str2d5=floatToString("",sum_pm2d5_net/spcount,4),strmq7=floatToString("",sum_mq7/spcount,4),strmq2=floatToString("",sum_mq2/spcount,4),strmq9=floatToString("",sum_mq9/spcount,4),strwet=floatToString("",sum_wet/spcount,4),strtemp=floatToString("",sum_Temp/spcount,4),strpress=floatToString("",sum_pressure/spcount,4);
//floatToString(str135,sum_mq135/spcount);

String args="pass=m62078&mq135="+str135+"&pm2d5="+str2d5+"&mq7="+strmq7+"&mq2="+strmq2+"&mq9="+strmq9+"&pressure="+strpress+"&humidity="+strwet+"&temperature="+strtemp+"&locat="+locat+"&pm2d5_pcs="+str2d5_pcs+"&pm10="+strpm10+"&pm10_pcs="+strpm10_pcs+"&luminance="+strlux+"&pm1_pcs="+strpm1_pcs+"&pm1="+strpm1+"&pm0d3_pcs="+strpm0d3+"&pm5_pcs="+strpm5;
//add more things into it.


//String args="pass=m62078&mq135="+str135+"&pm2d5="+str2d5+"&mq7="+strmq7+"&mq2="+strmq2+"&mq9="+strmq9+"&pressure="+strpress+"&humidity="+strwet+"&temperature="+strtemp+"&longitude="+longitute+"&latitude="+latitude+"&altitude="+height;
sim908_http("58.63.232.138:62078/env/input.php?"+args);
//sim908_http("www.denghaoqing.com/wendu/test.txt");

spcount=0;
sum_pm2d5_net=0;
sum_mq135=0;
sum_mq7=0;
sum_mq2=0;
sum_mq9=0;
sum_pressure=0;
sum_Temp=0;
sum_wet=0;

}

String cur2d5=floatToString("",cur_pm2d5,1),cur135=floatToString("",mq135,1),curtmp=floatToString("",air_Temp,1),curmq7=floatToString("",mq7,1),curmq2=floatToString("",mq2,1),curpress=floatToString("",pressure,2),curwet=floatToString("",air_Wet,1),curmq9=floatToString("",mq9,1);
//Show_CH_Font16(110,0,12,titlechs,0, 0, 0,255,255,255);
cleanscr();
myGLCD.setColor(0, 0, 0);
myGLCD.setBackColor(255, 255, 255);
setPM2d5(cur_pm2d5);
myGLCD.print("PM2.5="+cur2d5+"ug/m3     ", LEFT, 20);
setLevel(mq135);
myGLCD.print("Air Pullution="+cur135+"ppm   ", LEFT, 32);
myGLCD.setColor(0,0,0);
myGLCD.setBackColor(255, 255, 255);
myGLCD.print("Temperature="+curtmp+"C      ", LEFT, 44);
setLevel(mq7);
myGLCD.print("CO="+curmq7+"ppm        ", LEFT, 56);
setLevel(mq2);
myGLCD.print("Sulfur dioxide="+curmq2+"ppm        ", LEFT, 68);
myGLCD.setColor(0,0,0);
myGLCD.setBackColor(255, 255, 255);
myGLCD.print("Pressure="+curpress+"hPa      ", LEFT, 80);
myGLCD.print("Humidity="+curwet+"%         ", LEFT, 92);
setLevel(mq9);
myGLCD.print("Formaldehyde="+curmq9+"ppm        ", LEFT, 104);
myGLCD.print("====PAGE 1/2====", LEFT, 140);
ATTest();
//105
//Serial1.print("pm2.5:%f,air_Temp:%f,air_Wet:%f",pm2d5,air_Temp,air_Wet);
//delay(400);
gpsLocate();
delay(3000);
//****************************************************//
cleanscr();
myGLCD.setColor(0,0,0);
myGLCD.setBackColor(255, 255, 255);
myGLCD.print("PM1="+strpm1+"ug/m3               ", LEFT, 20);
//setLevel(cur_pm1);
myGLCD.print("PM10="+strpm10+"ug/m3           ", LEFT, 32);
myGLCD.setColor(0,0,0);
myGLCD.setBackColor(255, 255, 255);
myGLCD.print("count_0.3um="+strpm0d3+"pcs/dL     ", LEFT, 44);
//setLevel(mq7);
myGLCD.print("count_1.0um="+strpm1_pcs+"pcs/dL     ", LEFT, 56);
//setLevel(mq2);
myGLCD.print("count_2.5um="+str2d5_pcs+"pcs/dL        ", LEFT, 68);
//myGLCD.setColor(0,0,0);
//myGLCD.setBackColor(255, 255, 255);
myGLCD.print("count_5um="+strpm5+"pcs/dL        ", LEFT, 80);
myGLCD.print("count_10um="+strpm10_pcs+"pcs/dL          ", LEFT, 92);
//setLevel(mq9);
myGLCD.print("Brightness="+strlux+"lux        ", LEFT, 104);
myGLCD.print("====PAGE 2/2====", LEFT, 140);
//****************************************************//
//set normal color
myGLCD.setColor(0,0,0);
myGLCD.setBackColor(255, 255, 255);
ATTest();
//105
//Serial1.print("pm2.5:%f,air_Temp:%f,air_Wet:%f",pm2d5,air_Temp,air_Wet);
//delay(400);
gpsLocate();
/////TestCode
/*
digitalWrite(ledPower,HIGH);
delayMicroseconds(280);
String curvo=floatToString("",analogRead(pm25data),4);
delayMicroseconds(40);
digitalWrite(ledPower,LOW);
delayMicroseconds(9680);
myGLCD.print(curvo, LEFT, 125);*/
/////end test

//float to string 
if(errCount==50){
sim908_gsm_init();

gpsInit();

errCount=0;
}
}
/////////////end loop//////////////////////

/*
-------char=>string---------
String comdata
   while (Serial1.available() > 0)  
    {
        comdata += char(Serial1.read());
        delay(2);
    }
-------float=>string--------
*/

void ATTest(){
  
  myGLCD.setColor(0, 0, 0);
myGLCD.setBackColor(255, 255, 255);
while(Serial1.available()>0){
Serial1.read();
}
Serial1.println("AT");
delay(100);
int a=0;
char tmpAT;
while(Serial1.available()>0){
tmpAT=Serial1.read();
  if(tmpAT=='O'){
    Serial.println("DEBUG:excuted here");
    myGLCD.print("SIM908 STATUS OK", LEFT, 116);//116
    a=1;
  }
delay(2);
}
if(a==0){
     myGLCD.setColor(255, 0, 0);
    myGLCD.print("SIM908 STATUS NG", LEFT, 116);
}
     myGLCD.setColor(0, 0, 0);
  //   myGLCD.print("SIM908 DEBUG OK", LEFT, 116);//116
}

//获取pm2.5的数值


void updateWet(){
int chk = DHT.read11(DHT11_PIN); 
air_Temp=DHT.temperature;
air_Wet=DHT.humidity;
}

void sim908_gsm_init(){
//gsm，gprs初始化
delay(100);
Serial1.println("AT");
delay(100);
Serial1.println("AT+CSTT");
delay(400);
Serial1.println("AT+CIICR");
delay(1500);
Serial1.println("AT+CIFSR");
delay(200);
Serial1.println("AT+SAPBR=1,1");
delay(2000);
Serial1.println("AT+CSQ");
delay(100);
Serial1.println("AT+CREG?");
delay(100);
Serial1.println("AT+CGATT?;");
delay(100);
Serial1.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
delay(100);
Serial1.println("AT+SAPBR=3,1,\"APN\",\"CMNET\"");
delay(100);
}


void sim908_http(String url){
//http get函数
//
sim908_gsm_init();//just in case.
//

Serial1.println("AT+HTTPTERM");
delay(100);
Serial1.println("AT+HTTPINIT");
delay(100);
Serial1.println("AT+HTTPPARA=\"URL\",\""+url+"\"");
delay(500);
Serial1.println("AT+HTTPACTION=0");
delay(8000);
Serial1.println("AT+HTTPREAD");
delay(100);
Serial1.println("AT+HTTPTERM");
delay(100);
}
void gpsInit(){
 Serial1.println("AT");
delay(300);
Serial1.println("AT+CGPSPWR=1");
delay(1000);
Serial1.println("AT+CGPSRST=0");
delay(1000);
Serial1.println("AT+CGPSRST=1");
delay(3000); 
}
void gpsLocate(){
delay(100);
while(Serial1.available()>0){
Serial1.read();
}
Serial1.println("AT+CGPSSTATUS?");

//未定位死循环注释
int isLocated=0;

Serial1.println("AT+CGPSSTATUS?");
delay(150);
int notAvail=0;
char tmpGPS;
while(Serial1.available()>0){
tmpGPS=Serial1.read();
if(tmpGPS=='U'){
notAvail=1;
}else if(tmpGPS=='3'){
notAvail=3;
}else if(tmpGPS=='2'){
notAvail=2;
}
delay(2);
}
/*
  for(int i = 0; i < comdata.length() ; i++)
    {
      if(tmpGPS[i] == 'U')
      {
        j++;
      }
      else
      {
        numdata[j] = numdata[j] * 10 + (comdata[i] - '0');
      }
    }*/


myGLCD.print("                                   ", LEFT, 128);



if(notAvail==1){
    myGLCD.setColor(255, 0, 0);
myGLCD.setBackColor(255, 255, 255);
   myGLCD.print("GPS:Not Located Yet", LEFT, 128);
      errCount=errCount +1;
}else if(notAvail==0){
      myGLCD.setColor(255, 255, 0);
myGLCD.setBackColor(255, 255, 255);
myGLCD.print("GPS STAT Unknown", LEFT, 128);
errCount=0;
}else if(notAvail==2){
      myGLCD.setColor(0, 255, 0);
myGLCD.setBackColor(255, 255, 255);
myGLCD.print("Location 2D Fix", LEFT, 128);
errCount=0;
}else if(notAvail==3){
      myGLCD.setColor(0, 255, 0);
myGLCD.setBackColor(255, 255, 255);
myGLCD.print("Location 3D Fix", LEFT, 128);
errCount=0;
}
myGLCD.setColor(0, 0, 0);
delay(300);
}

///压敏传感器通讯函数
float getPressure(int type) 
{
  
  /* Get a new sensor event */ 
  sensors_event_t event;
  bmp.getEvent(&event);
 
  /* Display the results (barometric pressure is measure in hPa) */
  if (event.pressure && type==1)
  {
    /* Display atmospheric pressue in hPa */
    //Serial1.print("Pressure:    ");
    return event.pressure;//hpa
    //Serial1.println(" hPa");
    }else if(event.pressure && type==2){
    /* Calculating altitude with reasonable accuracy requires pressure    *
     * sea level pressure for your position at the moment the data is     *
     * converted, as well as the ambient temperature in degress           *
     * celcius.  If you don't have these values, a 'generic' value of     *
     * 1013.25 hPa can be used (defined as SENSORS_PRESSURE_SEALEVELHPA   *
     * in sensors.h), but this isn't ideal and will give variable         *
     * results from one day to the next.                                  *
     *                                                                    *
     * You can usually find the current SLP value by looking at weather   *
     * websites or from environmental information centers near any major  *
     * airport.                                                           *
     *                                                                    *
     * For example, for Paris, France you can check the current mean      *
     * pressure and sea level at: http://bit.ly/16Au8ol                   */
     
    /* First we get the current temperature from the BMP085 */
    float temperature;
    bmp.getTemperature(&temperature);
    //Serial1.print("Temperature: ");
//air_Temp = temperature;
    return temperature;
    //Serial1.println(" C");
}else if(event.pressure && type==3){
    /* Then convert the atmospheric pressure, and SLP to altitude         */
    /* Update this next line with the current SLP for better results      */
    float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;
    //Serial1.print("Altitude:    "); 
    return bmp.pressureToAltitude(seaLevelPressure,event.pressure); 
    //Serial1.println(" m");
    //Serial1.println("");
  }else{
    return 0;
  }
}

int setLevel(float value){
//for mq
if(value==0)
{
//4levels+2exceptions
return 0;//N/A
}else if(value<30){
myGLCD.setColor(31,99,11);
myGLCD.setBackColor(255, 255, 255);
return 0;//excellent
}else if(value >= 30 && value < 60){
myGLCD.setColor(163,153,18);
myGLCD.setBackColor(255, 255, 255);
return 0;//good
}else if(value >= 60 && value <= 100){
myGLCD.setColor(138,7,7);
myGLCD.setBackColor(255, 255, 255);
return 0;//soso
}else if(value > 130){
myGLCD.setColor(255,255,255);
myGLCD.setBackColor(255,0,0);
return 0;//bad :(
}else{
return 0;//err code
}
}
int setPM2d5(float value){
//for pm2d5
if(value==0)
{
//4levels+2exceptions
return 0;//N/A
}else if(value<50){
myGLCD.setColor(31,99,11);
myGLCD.setBackColor(255, 255, 255);
return 0;//excellent
}else if(value >= 50 && value < 100){
myGLCD.setColor(163,153,18);
myGLCD.setBackColor(255, 255, 255);
return 0;//good
}else if(value >= 100 && value <= 140){
myGLCD.setColor(138,7,7);
myGLCD.setBackColor(255, 255, 255);
return 0;//soso
}else if(value > 140){
myGLCD.setColor(255,255,255);
myGLCD.setBackColor(255,0,0);
return 0;//bad :(
}else{
return 0;//err code
}
}

void updateLocation(){
  while(Serial1.available()>0){
   Serial1.read(); 
  }
Serial1.println("AT+CGPSINF=0");
delay(300);
if(Serial1.available()>0){
locat="";
int readcol;

char tmpChar;
while(not(tmpChar==',')){
  tmpChar=Serial1.read();//清除
}

for(readcol=0;readcol<45;readcol++){
locat+=char(Serial1.read());
}
while(Serial1.available()>0){
Serial1.read();
}
}
//Serial1.println(locat);
//0,11318.690505,2305.366901,94.318646,201456598
/*
for(count=1;count<12;count++){
longitute+=char(Serial1.read());
}
Serial1.read();
for(count=1;count<12;count++){
latitude+=char(Serial1.read());
}
Serial1.read();
for(count=1;count<9;count++){
height+=char(Serial1.read());
}
while(Serial1.available()>0){
char tmp;
tmp=Serial1.read();
}*/
}
void getlight(){
lux = LightSensor.GetLightIntensity();
Serial.print("I/Brightness Sensor:");
Serial.println(lux);
}
void cleanscr(){
 myGLCD.setColor(0,0,0);
myGLCD.setBackColor(255, 255, 255);
myGLCD.print("                                    ", LEFT, 20);
myGLCD.print("                                    ", LEFT, 32);
myGLCD.print("                                    ", LEFT, 44);
myGLCD.print("                                    ", LEFT, 56);
myGLCD.print("                                    ", LEFT, 68);
myGLCD.print("                                    ", LEFT, 80);
myGLCD.print("                                    ", LEFT, 92);
myGLCD.print("                                    ", LEFT, 104);
myGLCD.print("                                    ", LEFT, 140); 
}


