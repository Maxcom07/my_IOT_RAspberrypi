#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "credentials.h"
#include "globals.h"

//MQTT
String clientID = "ESP32-";
const char* mqttServer = "192.168.1.109"; 
const char* mqttUser = "sompoch";
const char* mqttPassword = "45301074EE"; 
WiFiClient espClient;
PubSubClient client(espClient);


void reconnect()
{
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        clientID += String(random(0xffff), HEX);
        if (client.connect(clientID.c_str(), mqttUser,mqttPassword))
        {
            Serial.println("connect to MQTT");
            // client.subscribe("fromNodeRED");
            client.subscribe("pumpStart");
            // client.subscribe("stop");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // wait 5 seconds before retrying
            delay(5000);



        }
    }
}

void callback(char *topic, byte *message, unsigned int length)  // เป็นฟังก์ชันการรับข้อมูลจาก MQTT เพื่อมาดำเนินงานต่อ
{
// Serial.print("Message arrived on topic: ");
// Serial.print(topic);
// Serial.print(". Message: ");





String messageTemp;  //เป็นตัวแปรชนิด String ที่ใช้เก็บข้อความที่ได้รับจาก MQTT message.

for (int i = 0; i < length; i++)
{
    //Serial.print((char)message[i]);
    messageTemp += (char)message[i];   //อ่านข้อมูลจาก message ที่เป็นอาร์เรย์ของ byte แล้วเพิ่มไปยัง messageTemp โดยใช้ += เพื่อเชื่อมต่อข้อความที่ได้รับเข้าไปใน messageTemp
}

    if (String(topic) == "pumpStart")  //ถ้าข้อความจาก mqtt out (ผ่าน topic pumpStart) เท่ากับ pumpStart ให้ทำการ print ออก Serial Monitor VS code
    {
        if(messageTemp == "1" && Mode == 1) {
        digitalWrite(relaycontrolPumDRU, HIGH);
        client.publish("statusPump", "pump run manual"); 
        Serial.println("pump is run manual");

        previousMillis=millis();
        state=PUMPING;
        }
        else if(messageTemp == "0" && Mode == 1){
        digitalWrite(relaycontrolPumDRU, LOW);
        client.publish("statusPump", "pump stop manual"); 
        Serial.println("pump is stop manual");
        state= TIMEFORSLEEP;   
        }
        
    }
    if (String(topic) == "pumpStart")  //ถ้าข้อความจาก mqtt out (ผ่าน topic pumpmode  ใช้ dashboard switch node) 
    {
        if(messageTemp == "M"){
        Mode = 1;
	    Serial.println("statusPump is Manual");
        }
        else if(messageTemp  == "A"){
	    Mode = 0;
	    Serial.println("statusPump is Automatic");        
        }    
    }
}

void connectAP(){
    Serial.println("Connect to Sompoch WiFi");
    WiFi.begin(ssid, password);
    byte cnt=0;

    while(WiFi.status() != WL_CONNECTED){
        delay(1000);
        Serial.print(".");
        cnt++;

        if(cnt>30){
            ESP.restart();
        }


    }

}