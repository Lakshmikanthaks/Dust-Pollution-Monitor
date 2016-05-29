#include <SD.h> //Load SD card library
#include<SPI.h> //Load SPI Library
 
#include "Wire.h"    
// int sensor;
const int chipSelect = 4;
// int analogPin=0;

int measurePin1 = 2;
int measurePin2 = 3; 
int measurePin3 = 6;

int ledPower = 8;

int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;

float voMeasured1 = 0;
float calcVoltage1 = 0;
float dustDensity1 = 0;

float voMeasured2 = 0;
float calcVoltage2 = 0;
float dustDensity2 = 0;

float voMeasured3 = 0;
float calcVoltage3 = 0;
float dustDensity3 = 0;


  int Gpsdata;             // for incoming serial data
  unsigned int finish =0;  // indicate end of message
  unsigned int pos_cnt=0;  // position counter
  unsigned int lat_cnt=0;  // latitude data counter
  unsigned int log_cnt=0;  // longitude data counter
  unsigned int flg    =0;  // GPS flag
  unsigned int com_cnt=0;  // comma counter
  char lat[20];            // latitude array
  char lg[20];             // longitude 

File mySensorData; //Data object you will write your sensor data to

void Receive_GPS_Data();

void setup(){
Serial.begin(9600); //turn on serial monitor
pinMode(ledPower,OUTPUT);
pinMode(10, OUTPUT); //Must declare 10 an output and reserve it
SD.begin(4); //Initialize the SD card reader
}
 
void loop() {
 
  digitalWrite(ledPower,LOW); // power on the LED
  delayMicroseconds(samplingTime);

  voMeasured1 = analogRead(measurePin1); // read the dust value from first sensor 
  voMeasured2 = analogRead(measurePin2); // read the dust value from second sensor 
  voMeasured3 = analogRead(measurePin3); // read the dust value from third sensor 
  
  delayMicroseconds(deltaTime);
  digitalWrite(ledPower,HIGH); // turn the LED off
  delayMicroseconds(sleepTime);

  // 0 - 5.0V mapped to 0 - 1023 integer values 
  calcVoltage1 = voMeasured1 * (5.0 / 1024); 
  calcVoltage2 = voMeasured2 * (5.0 / 1024); 
  calcVoltage3 = voMeasured3 * (5.0 / 1024); 
  
  dustDensity1 = (0.17 * calcVoltage1 - 0.1)*1000; 
  dustDensity2 = (0.17 * calcVoltage2 - 0.1)*1000; 
  dustDensity3 = (0.17 * calcVoltage3 - 0.1)*1000; 
 
   String dataString;
   
    dataString += lat;
    dataString += ",";
    dataString += lg;
    dataString += ",";
    dataString += dustDensity1;
    dataString += ",";
    dataString += dustDensity2;
    dataString += ",";
    dataString += dustDensity3;
    //dataString += ",";        
    delay(1000);

  File dataFile = SD.open("dust.txt", FILE_WRITE);


  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }  

  else {
    Serial.println("error opening datalog.txt");
  } 

      Receive_GPS_Data();              
      /* Serial.println(lat);
      Serial.print(",");
      Serial.println(lg);
      Serial.print(",");
      Serial.print("Dust Density [ug/m3] of 1, 2 and 3: ");
      Serial.print(dustDensity1);
      Serial.print("\t");
      Serial.print(dustDensity2); 
      Serial.print("\t");
      Serial.println(dustDensity3); */      
      finish = 0;pos_cnt = 0;
}

 void Receive_GPS_Data()
  {
    while(finish==0){
      while(Serial.available()>0){         // Check GPS data
        Gpsdata = Serial.read();
        flg = 1;
       if( Gpsdata=='$' && pos_cnt == 0)   // finding GPRMC header
         pos_cnt=1;
       if( Gpsdata=='G' && pos_cnt == 1)
         pos_cnt=2;
       if( Gpsdata=='P' && pos_cnt == 2)
         pos_cnt=3;
       if( Gpsdata=='R' && pos_cnt == 3)
         pos_cnt=4;
       if( Gpsdata=='M' && pos_cnt == 4)
         pos_cnt=5;
       if( Gpsdata=='C' && pos_cnt==5 )
         pos_cnt=6;
       if(pos_cnt==6 &&  Gpsdata ==','){   // count commas in message
         com_cnt++;
         flg=0;
       }
 
       if(com_cnt==3 && flg==1){
        lat[lat_cnt++] =  Gpsdata;         // latitude
        flg=0;
       }
 
       if(com_cnt==5 && flg==1){
         lg[log_cnt++] =  Gpsdata;         // Longitude
         flg=0;
       }
 
       if( Gpsdata == '*' && com_cnt >= 5){
         com_cnt = 0;                      // end of GPRMC message
         lat_cnt = 0;
         log_cnt = 0;
         flg     = 0;
         finish  = 1;
 
      }
    }
}
}
