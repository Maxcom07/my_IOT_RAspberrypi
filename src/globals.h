#include <Arduino.h>
#ifndef globals_h    //เป็นคำสั่ง preprocessor ที่ย่อมาจาก "if not defined"  ป้องกันการรวมไฟล์ซ้ำซ้อน
#define globals_h

unsigned long previousMillis = 0;   // Stores last time temperature was published
unsigned long currentMillis = millis();
const byte relaycontrolPumDRU = 20;  // pin 20 esp32

// output pin declaration ultrasonic
// const int trigPin = 27;
// const int echoPin = 14;

int trigger1=27;
int echo1=14;

long distance1=0;
//long distance2=0;

int Mode;
//global variable
// long duration;
// int Level;
// int pwmWave;  

// Set the motor driver pins
// const int freq = 5000;
// const int ledChannel = 0;
// const int resolution = 8;
// int DCMTOR = 4;
// int IN1 = 2; 


enum states{
  GETDATA,
  PUMPING,
  TIMEFORSLEEP,
  WAIT
} state;

#endif