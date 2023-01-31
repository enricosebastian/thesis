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

//const String myName = "BASE";
const String myName = "DRON";

SoftwareSerial HC12(txPin, rxPin); // (Green TX, Blue RX)
LinkedList<String> drones;
Servo escLeft;
Servo escRight;
StaticJsonDocument<200> received; //Only received strings need to be global variables...

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
  if(myName == "BASE") {
    forBase();
  } else {
    forDrone();
  }
}

void forBase() {
  if(Serial.available()) {
    Serial.println(Serial.read());
  }
}

void forDrone() {
  escRight.write(11);
  escLeft.write(11);
  delay(1000);
  escRight.write(0);
  escLeft.write(0);
  delay(1000);
//  if(Serial.available()) {
//    Serial.println(Serial.read());
//  }
}
