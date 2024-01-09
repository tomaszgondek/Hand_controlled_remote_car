#include <Arduino.h>
#include <RF24.h>
#include <SPI.h>

#define en1 5
#define dir1 4
#define en2 6
#define dir2 7

float data[3] = {0,0,0};
RF24 radio(9,10);
const byte address[6] = "00001";
float X, Y, Z = 0;

void rev()
{
  digitalWrite(en1, 100);
  digitalWrite(en2, 100);
  digitalWrite(dir1, 1);
  digitalWrite(dir2, 1);
}

void left()
{
  digitalWrite(en1, 25);
  digitalWrite(en2, 25);
  digitalWrite(dir1, 1);
  digitalWrite(dir2, 0); 
}

void right()
{
  digitalWrite(en1, 25);
  digitalWrite(en2, 25);
  digitalWrite(dir1, 0);
  digitalWrite(dir2, 1); 
}

void fwdleft()
{
  digitalWrite(en1, 100);
  digitalWrite(en2, 25);
  digitalWrite(dir1, 1);
  digitalWrite(dir2, 1);
}

void fwdright()
{
  digitalWrite(en1, 25);
  digitalWrite(en2, 100);
  digitalWrite(dir1, 1);
  digitalWrite(dir2, 1); 
}

void fwd()
{
  digitalWrite(en1, 100);
  digitalWrite(en2, 100);
  digitalWrite(dir1, 0);
  digitalWrite(dir2, 0); 
}

void stop()
{
  digitalWrite(en1, 0);
  digitalWrite(en2, 0);
  digitalWrite(dir1, 0);
  digitalWrite(dir2, 0); 
}

void setup() 
{
  pinMode(en1, OUTPUT);
  pinMode(en2, OUTPUT);
  pinMode(dir1, OUTPUT);
  pinMode(dir2, OUTPUT);
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  Serial.write("BEGIN \n");
}

void loop() 
{
  if(radio.available())
  {
    radio.read(&data, sizeof(data));
    X = data[1];
    Y = data[2];
    Serial.print(X);
    Serial.print(", ");
    Serial.print(Y);
    Serial.print("\n");

    if(X < -0.5 && Y < 0.6 && Y > -0.6)
    {
      right();
      Serial.print("R \n");
    }
    else if(X > 0.3 && Y < 0.6 && Y > -0.6)
    {
      left();
      Serial.print("L \n");
    }
    else if(Y < -0.5 && X > -0.5 && X < 0.35)
    {
      fwd();
      Serial.print("Fwd \n");
    }
    else if(Y > 0.15 && X > -0.5 && X < 0.35)
    {
      rev();
      Serial.print("Rev \n");
    }
    /*
    else if(X < -0.32 && Y < -0.7)
    {
      fwdright();
      Serial.print("Fwd R \n");
    }
    else if(X > 0.4 && Y < -0.66)
    {
      fwdleft();
      Serial.print("Fwd L \n");
    }
    else
    */
   else
    {
      stop();
      Serial.print("S \n");
    }
  }
  else
  {
    //Serial.print("Error \n");
    //delay(100);
  }
}