/*
 * openag_bpm180.h
 * A library for barometer
 *
 * Copyright (c) Howard Webb
 * Author     : Howard Webb
 * Create Time: 12/30/2016
 * Change Log : Modified from Seeed Barometer.h
 *
 * loovee 9-24-2014
 * Change all int to short, all unsigned int to unsigned short to fit some 32bit system
 *
 * The MIT License (MIT)
 *
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
#ifndef __BAROMETER_H__
#define __BAROMETER_H__

#include <Arduino.h>
#include <Wire.h>
#include <openag_module.h>
#include <ros.h> //this must be included, and be put first
#include <std_msgs/Float32.h>	

const unsigned char OSS = 0;
#define BMP085_ADDRESS 0x77

class Barometer : public Module  //note: inheritance from OpenAg module
{
    public:
    void begin();
    void update();
    bool getTemp(std_msgs::Float32 &msg);	
    bool getPressure(std_msgs::Float32 &msg);	
    bool getAltitude(std_msgs::Float32 &msg);	
    long PressureCompensate;
    float calcTemperature(unsigned short ut);
    long calcPressure(unsigned long up);
    float calcAltitude(float pressure);
    unsigned short bmp085ReadUT(void);
    unsigned long bmp085ReadUP(void);

    private:

    short ac1;
    short ac2;
    short ac3;
    unsigned short ac4;
    unsigned short ac5;
    unsigned short ac6;
    short b1;
    short b2;
    short mb;
    short mc;
    short md;
    char bmp085Read(unsigned char address);
    short bmp085ReadInt(unsigned char address);
    void writeRegister(short deviceAddress, byte address, byte val);
    short readRegister(short deviceAddress, byte address);
// Open Ag sensor stuff  
  bool _send_temperature;
  bool _send_pressure;
  bool _send_altitude;
  uint32_t _time_of_last_reading;
  const static uint32_t _min_update_interval = 2000;
//return data holders
  float _pressure;
  float _temp;
  float _altitude;
};

#endif
