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

String sentMessage = "";
String receivedMessage = "";

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
    char letter = Serial.read();
    if(letter == '\n') {
      sentMessage += '\n';
      Serial.print("sent: ");
      Serial.println(sentMessage);
      HC12.println(sentMessage);
      sentMessage = "";
    } else {
      sentMessage += letter;
    }
  }

  if(HC12.available()) {
    receivedMessage = HC12.readStringUntil('\n');
    Serial.print("received: ");
    Serial.println(receivedMessage);
    receivedMessage = "";
  }
}
