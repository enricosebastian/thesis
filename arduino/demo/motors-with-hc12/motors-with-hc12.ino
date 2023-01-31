#include <Arduino.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <LinkedList.h>
#include <Servo.h>
#include <Wire.h>

const int escLeftPin = 6;
const int escRightPin = 5;
const int txPin = A0; //green tx
const int rxPin = A1; //blue rx

const String myName = "BASE";
// const String myName = "DRON";

SoftwareSerial HC12(txPin, rxPin); // (Green TX, Blue RX)
LinkedList<String> drones;
Servo escLeft;
Servo escRight;
DynamicJsonDocument doc(1024); //Only received strings need to be global variables...

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);
  
  //ESC initialization
  escLeft.attach(escLeftPin,1000,2000);
  escRight.attach(escRightPin,1000,2000);

  escLeft.write(0);
  escRight.write(0);
}

void loop() {
  if(Serial.available()) {
    String message = Serial.readStringUntil("\n");
    Serial.print("sending: ");
    Serial.println(message);
    
    doc["message"] = message;
    serializeJson(doc, HC12);
  }

  if(HC12.available()) {
    DeserializationError error = deserializeJson(doc, HC12);
    if(error) {
      Serial.println("deserializeJson() failed");
      return;
    }
    String message = doc["message"];
    Serial.print("received: ");
    Serial.println(message);
  }
}
