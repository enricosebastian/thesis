// Ubuntu command line for serial port: sudo chmod a+rw /dev/ttyACM0
#include <Arduino.h>
#include <NeoSWSerial.h>

//Name here
const char myName[4] = "DRO1";
// const char myName[4] = "BASE";
// const char myName[4] = "BASE";
// const char myName[4] = "BASE";

//Constants (buttons)
const int detectionPin = 10;
const int recordingPin = 9;
const int btn = 7;
const int coordPin = 6;

// Rule: For ports, green = RX, blue = TX
// For modules/chips, green = TX, blue = RX
const int rxHc12 = A0; //green wire
const int txHc12 = A1; //blue wire
const int rxNano = A2; //green wire
const int txNano = A3; //blue wire
const int rxPin = A4;
const int txPin = A5;

// Waiting times
const int waitingTime = 5000;

// Only for base station
const int redLed = 13;
const int yellowLed = 12;
const int greenLed = 11;

const float distanceBetweenTags = 9.5;

//Booleans for logic
bool isConnected = false;
bool isDeployed = false;
bool hasStopped = true;
bool hasDetectedObject = false;

//millis time variables for storage
unsigned long startTime = 0;
unsigned long startTime2 = 0;

float currentX = 0;
float homeX = 0;
float savedX = 0;

float currentY = 0;
float homeY = 0;
float savedY = 0;

float d1 = 0;
float d2 = 0;

// SoftwareSerial(rxPin, txPin, inverse_logic)
NeoSWSerial HC12(rxHc12, txHc12);
NeoSWSerial Nano(rxNano, txNano);
NeoSWSerial Esp(rxPin, txPin);

char sentMessage[64] = "";

void setup() {
  Serial.begin(9600);

  Serial.print(myName);
  Serial.println(" version 16 has initialized.");
}

void loop() {
  if(hasSentSerially()) {

  }
}

bool hasSentSerially() {
  while(Serial.available()) {
    char letter = Serial.read();
    if(letter == '\n') {
      sentMessage[strlen(sentMessage)] = '\0';
      Serial.println(sentMessage);
      memset(sentMessage, '\0', sizeof(sentMessage));

      return true;
    } else {
      sentMessage[strlen(sentMessage)] = letter;
      sentMessage[strlen(sentMessage)] = '\0';
    }
  }

  return false;
}