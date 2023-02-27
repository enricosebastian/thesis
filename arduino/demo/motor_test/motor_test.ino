#include <Arduino.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <LinkedList.h>
#include <Servo.h>
#include <Wire.h>
#include <HMC5883L_Simple.h>

HMC5883L_Simple Compass;
/*
 * GY-273 Compass Module  ->  Arduino
 * VCC  -> VCC  (See Note Below)
 * GND  -> GND
 * SCL  -> A5, blue
 * SDA  -> A4, green
*/

SoftwareSerial HC12(8, 9); // (Green TX, Blue RX)
LinkedList<String> drones;
Servo escLeft;
Servo escRight;
StaticJsonDocument<200> received; //Only received strings need to be global variables...

// Name here
// const String myName = "BASE";
const String myName = "DRO1";

const int redLed = 13;
const int yellowLed = 12;
const int greenLed = 11;
const int detectionPin = 10;
const int escLeftPin = 6;
const int escRightPin = 5;
const int btn = 7;

const float minSpeed = 7;
const float maxSpeed = 90;

void setup() {
  Serial.begin(9600);
  escLeft.attach(escLeftPin,1000,2000);
  escRight.attach(escRightPin,1000,2000); // (white pin, start range, end range)
  escLeft.write(0);
  escRight.write(0);
}

void loop() {
  escLeft.write(11);
  escRight.write(11);
  escLeft.write(20);
  escRight.write(20);
  
}
