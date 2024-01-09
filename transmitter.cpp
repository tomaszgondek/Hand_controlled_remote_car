#include <Arduino.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>
#include "nRF24L01.h"
#include <SPI.h>

#define indicator 8
#define errorLed 7
#define smallButton 0
#define bigButton 1

int ADXL345 = 0x53;
float X, Y, Z, oX, oY, oZ = 0;
int mX, mY, mZ = 0;
float data[3] = {0, 0, 0};
const byte address[6] = "00001";
unsigned int buttonCounter = 0;
bool buttonFlag = 0;
bool startup = 1;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, indicator, NEO_RGB + NEO_KHZ800);
RF24 radio(6, 9);


void colorWipe(uint32_t c, uint8_t wait)
{
  for(uint16_t i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  } 
}

void buttonIterrupt()
{
  buttonFlag = true;
}

void setup() 
{
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

  pinMode(errorLed, OUTPUT);
  pinMode(smallButton, INPUT_PULLUP);
  pinMode(bigButton, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(smallButton), buttonIterrupt, RISING);

  strip.begin();
  strip.setBrightness(25);
  strip.show();

  Wire.begin();
  Wire.beginTransmission(ADXL345);
  Wire.write(0x2D);
  Wire.write(8);
  Wire.endTransmission();

  delay(10);
}

void loop() 
{ 
  Wire.beginTransmission(ADXL345);
  Wire.write(0x32);
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true);
  X = (Wire.read() | Wire.read() << 8);
  X = X / 256;
  Y = (Wire.read() | Wire.read() << 8);
  Y = Y / 256;
  Z = (Wire.read() | Wire.read() << 8);
  Z = Z / 256;
  mX = map(X*1000, -1200, 1300, 0, 255);
  mY = map(Y*1000, -1500, 1500, 0, 255);
  mZ = map(Z*1000, -1000, 1000, 0, 255);

  if(buttonFlag)
  {
    delay(1000);
    oX = X;
    oY = Y;
    oZ = Z;
    buttonFlag = false;
    digitalWrite(errorLed, HIGH);
    delay(100);
    digitalWrite(errorLed, LOW);
    startup = false;
  }

  if(mX > 254)
  {
    mX = 254;
  }
  if(mX < 0)
  {
    mX = 0;
  }
  if(mY > 254)
  {
    mY = 254;
  }
  if(mY < 0)
  {
    mY = 0;
  }
  colorWipe(strip.Color(mX, mY, mX/mY*10), 0);

  data[1] = X - oX;
  data[2] = Y - oY;
  data[3] = Z - oZ;

  if(startup == false)
  {
    if(radio.write(&data, sizeof(data)) == false)
    {
      delay(100);
    }
    delay(100);
  }
  
}