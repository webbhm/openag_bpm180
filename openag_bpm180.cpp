/*
 * openag_bpm180.cpp
 * A library for barometer
 *
 * Copyright (c) 2012 Howard Webb
  * Author     : Howard Webb
 * Create Time: 12/30/2016
 * Change Log : Modified from Seeed Barometer.cpp
 * 
 * loovee 9-24-2014
 * Change all int to short, all unsigned int to unsigned short to fit some 32bit system
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

#include <openag_bpm180.h>
#include <Wire.h>
#include <Arduino.h>

void Barometer::begin() {
    Wire.begin();
    Serial.print("Temperaturet: ");
    ac1 = bmp085ReadInt(0xAA);
    ac2 = bmp085ReadInt(0xAC);
    ac3 = bmp085ReadInt(0xAE);
    ac4 = bmp085ReadInt(0xB0);
    ac5 = bmp085ReadInt(0xB2);
    ac6 = bmp085ReadInt(0xB4);
    b1 = bmp085ReadInt(0xB6);
    b2 = bmp085ReadInt(0xB8);
    mb = bmp085ReadInt(0xBA);
    mc = bmp085ReadInt(0xBC);
    md = bmp085ReadInt(0xBE);
    Serial.print("Temperaturet2: ");
	status_level = OK;
    status_msg = "";
    _send_pressure = false;
	_send_temperature = false;
    _send_altitude = false;		
    _time_of_last_reading = 0;
}
//update data if old and set flags if good data
void Barometer::update() {
   if (millis() - _time_of_last_reading > _min_update_interval) {
      _temp = calcTemperature(bmp085ReadUT()); //Get the temperature, bmp085ReadUT MUST be called first
      _pressure = calcPressure(bmp085ReadUP());//Get the pressure
      _altitude = calcAltitude(_pressure); //Uncompensated caculation - in Meters
      _time_of_last_reading = millis();
      if (_temp > 0 && _pressure > 0){
        _send_pressure = true;
		_send_temperature = true;
        _send_altitude = true;		
        status_level = OK;
        status_msg = "";
      } else {
        status_level = ERROR;
        status_msg = "31";
      }
	}
	
}
//std ROS message packaging the data
bool Barometer::getTemp(std_msgs::Float32 &msg){
   msg.data = _temp;
   bool res = _send_temperature;
  _send_temperature = false;
  return res;
}

bool Barometer::getPressure(std_msgs::Float32 &msg){
   msg.data = _pressure;
   bool res = _send_pressure;
  _send_pressure = false;
  return res;
}
bool Barometer::getAltitude(std_msgs::Float32 &msg){
   msg.data = _altitude;
   bool res = _send_altitude;
  _send_altitude = false;
  return res;
}
// Read 1 byte from the BMP085 at 'address'
// Return: the read byte;
char Barometer::bmp085Read(unsigned char address)
{
    //Wire.begin();
    unsigned char data;
    Wire.beginTransmission(BMP085_ADDRESS);
    Wire.write(address);
    Wire.endTransmission();

    Wire.requestFrom(BMP085_ADDRESS, 1);
    while(!Wire.available());
    return Wire.read();
}

// Read 2 bytes from the BMP085
// First byte will be from 'address'
// Second byte will be from 'address'+1
short Barometer::bmp085ReadInt(unsigned char address)
{
    unsigned char msb, lsb;
    Wire.beginTransmission(BMP085_ADDRESS);
    Wire.write(address);
    Wire.endTransmission();
    Wire.requestFrom(BMP085_ADDRESS, 2);
    while(Wire.available()<2);
    msb = Wire.read();
    lsb = Wire.read();
    return (short) msb<<8 | lsb;
}

// Read the uncompensated temperature value
unsigned short Barometer::bmp085ReadUT()
{
    unsigned short ut;
    Wire.beginTransmission(BMP085_ADDRESS);
    Wire.write(0xF4);
    Wire.write(0x2E);
    Wire.endTransmission();
    delay(5);
    ut = bmp085ReadInt(0xF6);
    return ut;
}
// Read the uncompensated pressure value
unsigned long Barometer::bmp085ReadUP()
{
    unsigned char msb, lsb, xlsb;
    unsigned long up = 0;
    Wire.beginTransmission(BMP085_ADDRESS);
    Wire.write(0xF4);
    Wire.write(0x34 + (OSS<<6));
    Wire.endTransmission();
    delay(2 + (3<<OSS));

    // Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
    msb = bmp085Read(0xF6);
    lsb = bmp085Read(0xF7);
    xlsb = bmp085Read(0xF8);
    up = (((unsigned long) msb << 16) | ((unsigned long) lsb << 8) | (unsigned long) xlsb) >> (8-OSS);
    return up;
}

void Barometer::writeRegister(short deviceAddress, byte address, byte val)
{
    Wire.beginTransmission(deviceAddress); // start transmission to device
    Wire.write(address);       // send register address
    Wire.write(val);         // send value to write
    Wire.endTransmission();     // end transmission
}

short Barometer::readRegister(short deviceAddress, byte address)
{
    short v;
    Wire.beginTransmission(deviceAddress);
    Wire.write(address); // register to read
    Wire.endTransmission();

    Wire.requestFrom(deviceAddress, 1); // read a byte

    while(!Wire.available()) {
        // waiting
    }

    v = Wire.read();
    return v;
}

float Barometer::calcAltitude(float pressure)
{
    float A = pressure/101325;
    float B = 1/5.25588;
    _altitude = pow(A,B);
    _altitude = 1 - _altitude;
    _altitude = _altitude /0.0000225577;
    return _altitude;
}

float Barometer::calcTemperature(unsigned short ut)
{
    long x1, x2;

    x1 = (((long)ut - (long)ac6)*(long)ac5) >> 15;
    x2 = ((long)mc << 11)/(x1 + md);
    PressureCompensate = x1 + x2;

    _temp = ((PressureCompensate + 8)>>4);
    _temp = _temp /10;

    return _temp;
}

//pressure in Pascals
// mb = p * 0.01  millibars
// mb * 0.401865  inches H20
// mb * 0.02953   inches Hg
long Barometer::calcPressure(unsigned long up) {
    long x1, x2, x3, b3, b6, p;
    unsigned long b4, b7;
    b6 = PressureCompensate - 4000;
    x1 = (b2 * (b6 * b6)>>12)>>11;
    x2 = (ac2 * b6)>>11;
    x3 = x1 + x2;
    b3 = (((((long)ac1)*4 + x3)<<OSS) + 2)>>2;

    // Calculate B4
    x1 = (ac3 * b6)>>13;
    x2 = (b1 * ((b6 * b6)>>12))>>16;
    x3 = ((x1 + x2) + 2)>>2;
    b4 = (ac4 * (unsigned long)(x3 + 32768))>>15;

    b7 = ((unsigned long)(up - b3) * (50000>>OSS));
    if (b7 < 0x80000000)
    p = (b7<<1)/b4;
    else
    p = (b7/b4)<<1;

    x1 = (p>>8) * (p>>8);
    x1 = (x1 * 3038)>>16;
    x2 = (-7357 * p)>>16;
    p += (x1 + x2 + 3791)>>4;

    _pressure = p;
    return _pressure;
}