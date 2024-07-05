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

void startPumping() {
  //unsigned long currentMillisPumpen = millis();  
  getDistance();
  Serial.println(distance1);
  

  if (distance1 >= 20 && Mode == 0 ) {
    // ถ้าระยะทางเท่ากับ 10 เซนติเมตร ให้เปิดปั๊ม
    digitalWrite(relaycontrolPumDRU, HIGH);
    client.publish("statusPump", "pump is On Automatic");
    Serial.println("pump is On Automatic");
  } else if (distance1 <= 20 && Mode == 0) {
    // ถ้าระยะทางเท่ากับ 30 เซนติเมตร ให้ปิดปั๊ม
    digitalWrite(relaycontrolPumDRU, LOW);
    client.publish("statusPump", "pump is off Automatic");
    Serial.println("pump is off Automatic");
    // เปลี่ยนสถานะเป็น TIMEFORSLEEP
    state = TIMEFORSLEEP;
  }

    // ส่งข้อมูลปั๊มใหม่ไปยังที่ที่ต้องการ
    sendNewPumpInfo();
    

  
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

  
}

void senTemptoNodered(float temperature) {
 
  // สร้าง JSON document
  StaticJsonDocument<48> doc;   // เลข 48 หมายถึงขนาดของหน่วยความ 48 bytes ที่จองใว้เพื่อ json
  doc["temperature"] = temperature;

  char buffer[48];
  serializeJson(doc, buffer);

  // ส่งข้อมูลผ่าน MQTT
  client.publish("ds18b20/temperature", buffer);

  if (client.publish("ds18b20/temperature", buffer)) {
    Serial.println("Temperature sent successfully");
    //state = WAIT;
  } else {
    Serial.println("Temperature send failed");
  }
}


void loop() {
  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop()){
    client.connect("ESP32-"); // mqtt connect
  }

  startPumping(); 

  sensors.requestTemperatures(); //สั่งอ่านค่าอุณหภูมิ
  float temperature = sensors.getTempCByIndex(0); // อ่านค่าอุณหภูมิ celcius
  Serial.print(temperature);
  Serial.println(" *C");
  delay(2000);

  unsigned long currentMillis = millis();  //  สาเหตุที่ MQTT ไม่ออกเกิดจากลืมใส่ code นี้
  if (currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis;
    // Request temperature readings
    sensors.requestTemperatures();
    // Read temperature in Celsius degrees
    float tempC = sensors.getTempCByIndex(0); 
    //client.publish("ds18b20/temperature","sensors ");
    //client.publish("Watertank/Level","Level ");
    senTemptoNodered(tempC);
    delay(500);
    // ปั้มทำงานอัตโนมัติ

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





}
}