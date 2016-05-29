#include <SD.h> //Load SD card library
#include<SPI.h> //Load SPI Library
 
 
#include "Wire.h"    // imports the wire library for talking over I2C 
int sensor;
const int chipSelect = 4;
int analogPin=0;

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
 
pinMode(10, OUTPUT); //Must declare 10 an output and reserve it
SD.begin(4); //Initialize the SD card reader
}
 
void loop() {
 

   String dataString;
   
    dataString += lat;
    dataString += ",";
    dataString += lg;
    dataString += ",";
   for (analogPin = 0; analogPin < 5; analogPin++){
    //reads the sensor on PIN0 to 4 
      sensor = analogRead(analogPin);
    
   //stores our values
    dataString += String(sensor);
    if (analogPin < 5) {
      dataString += ","; 
    } 
   }

    // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open("data.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  } 

      Receive_GPS_Data();              
      /* Serial.println(lat);
      Serial.print(",");
      Serial.println(lg);
      Serial.print(",");
      Serial.println(analogRead(A0));
      Serial.print(",");
      Serial.println(analogRead(A1));
      Serial.print(",");
      Serial.println(analogRead(A2));
      Serial.print(",");
      Serial.println(analogRead(A3));
      Serial.print(",");
      Serial.println(analogRead(A4));
      Serial.print(","); */ 
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
