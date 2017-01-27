#include <openag_bpm180.h>

/*
 * Barometer_Sensor.ino
 * Example sketch for barometer
 *
 * Copyright (c) 2012 Howard Webb
 * Author     : Howard Webb
 * Create Time: 12/30/2016
 * Change Log : Modified from the Seed code
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <Wire.h>
#include <openag_module.h>
#include <ros.h> //this must be included, and be put first
//#include <std_msgs/UInt16.h>  
#include <std_msgs/Float32.h>
float temperature;
float pressure;
float pres_mb;
float pres_h20;
float pres_hg;
float atm;
float altitude;
bool status;
std_msgs::Float32 msg;
Barometer myBarometer;

void setup(){
  Serial.begin(9600);
  myBarometer.set_Address(119);
  myBarometer.begin();
}

void loop()
{
  bool old_data = true;

  myBarometer.update();
  old_data = myBarometer.get_Temp(msg);
  if (old_data){
    Serial.println("Old Temp");
  } 
  else {
    Serial.println("Fresh Temp");
  }

  temperature = msg.data;

  old_data = myBarometer.get_Pressure(msg);
  if (old_data){
    Serial.println("Old Pressure");
  } 
  else {
    Serial.println("Fresh Pressure");
  }

  pressure = msg.data;

  old_data = myBarometer.get_Altitude(msg);
  if (old_data){
    Serial.println("Old Altitude");
  } 
  else {
    Serial.println("Fresh Altitude");
  }


  altitude = msg.data;
  /*   
   Serial.println("Low Level Test:");
   Serial.print("Temp: ");
   Serial.println(myBarometer.calcTemperature(myBarometer.bmp085ReadUT())); //Get the temperature, bmp085ReadUT MUST be called first
   Serial.print("Pres: ");
   Serial.println(myBarometer.calcPressure(myBarometer.bmp085ReadUP()));//Get the temperature
   Serial.print("Alt: ");
   Serial.println(myBarometer.calcAltitude(pressure)); //Uncompensated caculation - in Meters
   Serial.println();
   */


  atm = pressure / 101325;
  pres_mb = pressure * 0.01;
  pres_h20 = pres_mb * 0.401865;
  pres_hg = pres_mb * 0.02953;


  Serial.print("Temperature: ");
  Serial.print(temperature, 2); //display 2 decimal places
  Serial.println(" deg C");

  Serial.print("Pressure: ");
  Serial.print(pressure, 0); //whole number only.
  Serial.println(" Pa");

  Serial.print("Pressure: ");
  Serial.print(pres_mb, 2); //whole number only.
  Serial.println(" mb");

  Serial.print("Pressure: ");
  Serial.print(pres_h20, 2); //whole number only.
  Serial.println(" inch H2O");

  Serial.print("Pressure: ");
  Serial.print(pres_hg, 2); //whole number only.
  Serial.println(" inch Hg");

  Serial.print("Ralated Atmosphere: ");
  Serial.println(atm, 4); //display 4 decimal places

  Serial.print("Altitude: ");
  Serial.print(altitude, 2); //display 2 decimal places
  Serial.println(" m");

  Serial.println();

  delay(1000); //wait a second and get values again.
}




