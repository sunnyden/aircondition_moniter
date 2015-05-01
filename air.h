/*
file:airpollution.h
author:sunnyden
颗粒物---->>>txrx1

*/

byte dat;
byte RxBuffer[24];
int i = 0;
int recflag_DEV;
int recfinish_DEV;
float cur_pm1 = 0;
float cur_pm2d5 = 0;
float cur_pm10 = 0;
float cur_pm0d3_pcs = 0;
float cur_pm0d5_pcs = 0;
float cur_pm1_pcs = 0;
float cur_pm2d5_pcs = 0;
float cur_pm5_pcs = 0;
float cur_pm10_pcs = 0; //实时查看
float sum_pm1 = 0;
float pm1 = 0;
float sum_pm2d5 = 0;
float pm2d5 = 0;
float sum_pm10 = 0;
float pm10 = 0;
float sum_pm0d3_pcs = 0;
float pm0d3_pcs = 0;
float sum_pm0d5_pcs = 0;
float pm0d5_pcs = 0;
float sum_pm1_pcs = 0;
float pm1_pcs = 0;
float sum_pm2d5_pcs = 0;
float pm2d5_pcs = 0;
float sum_pm5_pcs = 0;
float pm5_pcs = 0;
float sum_pm10_pcs = 0;
float pm10_pcs = 0;
int Ok_num = 0;
void serialinit() {
  Serial.begin(9600);
  Serial.println("Mobile Air pollution measuring system");
  Serial.println("Starting up....");

}
void dirtsetup()
{
  Serial.begin(9600);
  Serial2.begin(9600);
  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);

}
void update_pm2d5() {
  /*
  serial location #2 txrx1
  从全局变量中读取：
      	pm1=sum_pm1/Ok_num;
        pm2d5=sum_pm2d5/Ok_num;
        pm10=sum_pm10/Ok_num;
        pm0d3_pcs=sum_pm0d3_pcs/Ok_num;
        pm0d5_pcs=sum_pm0d5_pcs/Ok_num;
        pm1_pcs=sum_pm1_pcs/Ok_num;
        pm2d5_pcs=sum_pm2d5_pcs/Ok_num;
        pm5_pcs=sum_pm5_pcs/Ok_num;
        pm10_pcs=sum_pm10_pcs/Ok_num;


  */
  while (Serial2.available() > 0) {
    dat = Serial2.read();         //定义dat读取串口
    delay(2);


    //Serial.println("==========");
    if (dat == 66 && i == 0)        //找出第一个byte并赋值到RxBuffer数组
    {
      RxBuffer[i] = dat;
      i++;
      //Serial.println(i);
      recflag_DEV = 1;        //设置接收标识
    }
    else
    {
      if (recflag_DEV == 1) //如果接收标识为1，继续接收
      {
        if (i < 24) //如果小于传感器每节的数据数，就继续接收
        {
          RxBuffer[i] = dat;
          //Serial.println(i);
          i++;
          if ( i == 24)              //到了最后一个字段。
          {
            int k;
            int sum = 0;
            for (k = 0; k < 24; k++)
            { //测试
              if (k < 22)
              {
                sum += RxBuffer[k]; //叠加校验字段之前的所有数。
              }
              Serial.print("//");  //打印各字段
              Serial.print(RxBuffer[k], DEC); //打印各字段

            }
            if (sum == RxBuffer[22] * 256 + RxBuffer[23]) //校验，只选择校验正确的数据
            {
              Serial.print("checksum ok"); //最后在这位置获取每次扫描数据平均值，供其他程序用。
              cur_pm1 = RxBuffer[4] * 256 + RxBuffer[5];
              cur_pm2d5 = RxBuffer[6] * 256 + RxBuffer[7];
              cur_pm10 = RxBuffer[8] * 256 + RxBuffer[9];
              cur_pm0d3_pcs = RxBuffer[10] * 256 + RxBuffer[11];
              cur_pm0d5_pcs = RxBuffer[12] * 256 + RxBuffer[13];
              cur_pm1_pcs = RxBuffer[14] * 256 + RxBuffer[15];
              cur_pm2d5_pcs = RxBuffer[16] * 256 + RxBuffer[17];
              cur_pm5_pcs = RxBuffer[18] * 256 + RxBuffer[19];
              cur_pm10_pcs = RxBuffer[20] * 256 + RxBuffer[21];
                  Serial.print("average pm1=");
    Serial.println(cur_pm1);
    Serial.print("average pm2d5=");
    Serial.println(cur_pm2d5);
    Serial.print("average pm10=");
    Serial.println(cur_pm10);
    Serial.print("average pm0d3_pcs=");
    Serial.println(cur_pm0d3_pcs);
    Serial.print("average pm0d5_pcs=");
    Serial.println(cur_pm0d5_pcs);
    Serial.print("average pm1_pcs=");
    Serial.println(cur_pm1_pcs);
    Serial.print("average pm2d5_pcs=");
    Serial.println(cur_pm2d5_pcs);
    Serial.print("average pm5_pcs=");
    Serial.println(cur_pm5_pcs);
    Serial.print("average pm10_pcs=");
    Serial.println(cur_pm10_pcs);
    Serial.print("\n");
              //update currunt details
              sum_pm1 += RxBuffer[4] * 256 + RxBuffer[5];
              sum_pm2d5 += RxBuffer[6] * 256 + RxBuffer[7];
              sum_pm10 += RxBuffer[8] * 256 + RxBuffer[9];
              sum_pm0d3_pcs += RxBuffer[10] * 256 + RxBuffer[11];
              sum_pm0d5_pcs += RxBuffer[12] * 256 + RxBuffer[13];
              sum_pm1_pcs += RxBuffer[14] * 256 + RxBuffer[15];
              sum_pm2d5_pcs += RxBuffer[16] * 256 + RxBuffer[17];
              sum_pm5_pcs += RxBuffer[18] * 256 + RxBuffer[19];
              sum_pm10_pcs += RxBuffer[20] * 256 + RxBuffer[21];
              //                                                   Serial.print(Ok_num);
              Ok_num++;
            }
            else
            {
              Serial.print("checksum failed");
            }
            Serial.print("==end=="); Serial.println("");
            //Serial.println("----------loop ended");
            recflag_DEV = 0;	 //设置接收标识为0，表示不再接收。
            recfinish_DEV = 1; //设置接收完成标致，目前还没被调用。
            i = 0;           //i恢复到0

          }
        }
      }
    }




  } //while结束

  if ( Ok_num == 40)  //如果记录出现了20次，计算平均。
  {
    
    pm1 = sum_pm1 / Ok_num;
    pm2d5 = sum_pm2d5 / Ok_num;
    pm10 = sum_pm10 / Ok_num;
    pm0d3_pcs = sum_pm0d3_pcs / Ok_num;
    pm0d5_pcs = sum_pm0d5_pcs / Ok_num;
    pm1_pcs = sum_pm1_pcs / Ok_num;
    pm2d5_pcs = sum_pm2d5_pcs / Ok_num;
    pm5_pcs = sum_pm5_pcs / Ok_num;
    pm10_pcs = sum_pm10_pcs / Ok_num;

    Serial.print("average pm1=");
    Serial.println(pm1);
    Serial.print("average pm2d5=");
    Serial.println(pm2d5);
    Serial.print("average pm10=");
    Serial.println(pm10);
    Serial.print("average pm0d3_pcs=");
    Serial.println(pm0d3_pcs);
    Serial.print("average pm0d5_pcs=");
    Serial.println(pm0d5_pcs);
    Serial.print("average pm1_pcs=");
    Serial.println(pm1_pcs);
    Serial.print("average pm2d5_pcs=");
    Serial.println(pm2d5_pcs);
    Serial.print("average pm5_pcs=");
    Serial.println(pm5_pcs);
    Serial.print("average pm10_pcs=");
    Serial.println(pm10_pcs);
    Serial.print("\n");
    //  digitalWrite(7,LOW);
    //     delay(20000); 不循环了……
    digitalWrite(7, HIGH);
    delay(2);
    Ok_num = 0;

    sum_pm1 = 0;
    sum_pm2d5 = 0;
    sum_pm10 = 0;
    sum_pm0d3_pcs = 0;
    sum_pm0d5_pcs = 0;
    sum_pm1_pcs = 0;
    sum_pm2d5_pcs = 0;
    sum_pm5_pcs = 0;
    sum_pm10_pcs = 0;
    //清空，准备计算下一组数据
  }
}


void wendu() {
  /*
  wendu
  */





}
void light() {


}
