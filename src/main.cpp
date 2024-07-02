#include <Arduino.h>
#include <ArduinoJson.h>
#include "wifimqtt.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "globals.h"
#include "ultrasonic.h"

#define ONE_WIRE_BUS 13 
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

//ส่งข้อมูลไป MQTT Node red
void sendNewPumpInfo(){

StaticJsonDocument<48> doc;

doc["distance1"] = distance1;
doc["distance2"] = "22";

char buffer_1[256];
serializeJson(doc, buffer_1);
// ส่งข้อมูลผ่าน MQTT
client.publish("WaterDistance", buffer_1);
Serial.println("distance transmitted");

}



void startPumping(){

  unsigned long currentMillisPumpen = millis();  //กำหนดตัวแปรใหม่ เวลาปัจจุบัน
  digitalWrite(relaycontrolPumDRU, HIGH);
  if (currentMillisPumpen - previousMillis >= 1000 *3) 
{
    digitalWrite(relaycontrolPumDRU, LOW);
    Serial.println("pump is off");

  getDistance();
  Serial.println(distance1);
  sendNewPumpInfo();

  state=TIMEFORSLEEP;
}
}


void getData(){
  client.publish("pumpInfo", "Give me Information");
  Serial.println("mqtt publish pumpInfo - no wait");
  state = WAIT;
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  state=GETDATA;
  pinMode(relaycontrolPumDRU, OUTPUT);
  pinMode(trigger1, OUTPUT);
  pinMode(echo1, INPUT);

  connectAP();
  client.setServer(mqttServer,1883);
  client.setCallback(callback);

    // set ค่า Tx และ ค่า Rx ของ sensor ultrasonic  
  // pinMode(trigPin, OUTPUT);
  // Serial.begin(115200);
  // Serial.println("ultrasonic Level by Sompoch");
  // Serial.println("initializing ....");
  // Serial.println(ssid);

  // PWM signal output to Drive DCMotor
  // ledcSetup(ledChannel, freq, resolution);
  // ledcAttachPin(DCMTOR, ledChannel);
  // pinMode(IN1, OUTPUT);
  // digitalWrite(IN1, HIGH);

  
}

void senTemptoNodered(float temperature) {
 
  // สร้าง JSON document
  StaticJsonDocument<48> doc;  // เลข 48 หมายถึงขนาดของหน่วยความ 48 bytes ที่จองใว้เพื่อ json
  doc["temperature"] = temperature;

  char buffer[48];
  serializeJson(doc, buffer);

  // ส่งข้อมูลผ่าน MQTT
  client.publish("ds18b20/temperature", buffer);
}


void loop() {
  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop()){
    client.connect("ESP32-"); // mqtt connect
  }

  // Write a pulse to the HC-SR04 Trigger pin
  // digitalWrite(trigPin, LOW);
  // delayMicroseconds(2);
  // digitalWrite(trigPin, HIGH);
  // delayMicroseconds(10);
  // digitalWrite(trigPin, LOW);
  // // Measure the response from the HC-SR04 ECHOPIN
  // duration = pulseIn(echoPin, HIGH);
  // // เวลาของเสียงเดินทางในอากาศ 343 meters per second  แปลงจาก   343 meters/seconds  to centimeters/microseconds  
  // Level = duration * 0.034 / 2;  
  // ที่หารสอง เพราะว่าอาแค่เวลาเดินไปเท่านั้น
  // pwmWave = map(Level, 0, 100, 0, 255);
  // ledcWrite(ledChannel, pwmWave); //motor runing
  // Serial.println(pwmWave);

  // Sensors วัด Temperature DS18B20
  sensors.requestTemperatures(); //สั่งอ่านค่าอุณหภูมิ
  float temperature = sensors.getTempCByIndex(0); // อ่านค่าอุณหภูมิ celcius
  Serial.print(temperature);
  Serial.println(" *C");
  delay(2000);

 // แสดงระยะทางใน Serial Monitor
  // Serial.print(Level);
  // Serial.println("cm");

  delay(500);

  switch(state){
  case GETDATA:
    Serial.println("I want MQTT DATA");
    getData();
    break;
  case PUMPING:
    startPumping();
    break;
  case WAIT:  // ช่วงสถานะที่ หยุดนิ่งไม่ทำอะไรเลย
    break;
  case TIMEFORSLEEP: //pump go to sleep
    break; 
  }


 
  // Every X number of seconds (interval = 10 seconds) publish a new MQTT message
  if (currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis;
    // Request temperature readings
    sensors.requestTemperatures();
    // Read temperature in Celsius degrees
    float tempC = sensors.getTempCByIndex(0); 
    //client.publish("ds18b20/temperature","sensors ");
    //client.publish("Watertank/Level","Level ");
    senTemptoNodered(tempC);
    

}
}