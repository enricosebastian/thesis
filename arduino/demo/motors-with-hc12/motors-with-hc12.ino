#include <Arduino.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <LinkedList.h>
#include <Servo.h>
#include <Wire.h>

const int redLed = 13;
const int yellowLed = 12;
const int greenLed = 11;
const int btn = 7;
const int escLeftPin = 6;
const int escRightPin = 5;
const int txPin = A0; //green tx
const int rxPin = A1; //blue rx

//sent strings
String sentCommand = "";
String sentToName = "";
String sentFromName = "";
String sentDetails = "";
String sentMessage = "";

//received strings
String receivedCommand = "";
String receivedToName = "";
String receivedFromName = "";
String receivedDetails = "";
String receivedMessage = "";

SoftwareSerial HC12(txPin, rxPin); // (Green TX, Blue RX)
LinkedList<String> drones;
Servo escLeft;
Servo escRight;
DynamicJsonDocument doc(1024); //Only received strings need to be global variables...

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);

  //GPIO initialization
  pinMode(redLed, OUTPUT);
  digitalWrite(redLed, LOW);
  pinMode(yellowLed, OUTPUT);
  digitalWrite(yellowLed, LOW);
  pinMode(greenLed, OUTPUT);
  digitalWrite(greenLed, LOW);

  pinMode(btn, INPUT);
  
  //ESC initialization
  escLeft.attach(escLeftPin,1000,2000);
  escRight.attach(escRightPin,1000,2000);

  escLeft.write(0);
  escRight.write(0);
}

void loop() {
  if(digitalRead(btn) == HIGH) {
    pinMode(redLed, HIGH);
    pinMode(yellowLed, LOW);
    pinMode(greenLed, LOW);
    delay(1000);
    sentMessage = "COMM DRO1 YEET";
    HC12.print(sentMessage);
    pinMode(redLed, LOW);
    pinMode(yellowLed, LOW);
    pinMode(greenLed, HIGH);
  }

  if(HC12.available()) {
    receivedMessage = HC12.readStringUntil('\n');
    Serial.print("received: ");
    Serial.println(receivedMessage);
    interpretReceivedMessage(receivedMessage);
  }
}

void interpretReceivedMessage(String message) {
  int endIndex = message.indexOf(' ');
  receivedCommand = message.substring(0, endIndex);
  message = message.substring(endIndex+1);
  endIndex = message.indexOf(' ');
  receivedToName = message.substring(0, endIndex);
  receivedDetails = message.substring(endIndex+1);
  Serial.print("Received: {");
  Serial.print(receivedCommand);
  Serial.print(", ");
  Serial.print(receivedToName);
  Serial.print(", ");
  Serial.print(receivedDetails);
  Serial.println("}");
}
