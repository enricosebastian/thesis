// Ubuntu command line for serial port: sudo chmod a+rw /dev/ttyACM0
#include <Arduino.h>
#include <NeoSWSerial.h>
#include <LinkedList.h>

//Name here
const String myName = "BASE";
// const String myName = "DRO1";
// const String myName = "DRO2";
// const String myName = "DRO3";

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

//Booleans for logic
bool isConnected = false;
bool isDeployed = false;
bool hasStopped = true;
bool hasDetectedObject = false;

//millis time variables for storage
unsigned long startTime = 0;
unsigned long startTime2 = 0;

//received message
String receivedMessage = "";
String receivedCommand = "";
String receivedToName = "";
String receivedFromName = "";
String receivedDetails = "";

//sent message
String sentMessage = "";
String sentCommand = "";
String sentToName = "";
String sentFromName = "";
String sentDetails = "";

// SoftwareSerial(rxPin, txPin, inverse_logic)
NeoSWSerial HC12(rxHc12, txHc12);

LinkedList<String> drones;

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);

  Serial.print(myName);
  Serial.println(" is initializing...");
  
  //GPIO initialization
  pinMode(detectionPin, OUTPUT);
  pinMode(recordingPin, OUTPUT);
  pinMode(btn, INPUT);

  //Set output pins
  digitalWrite(detectionPin, LOW);
  digitalWrite(recordingPin, LOW);

  //Successful intialization indicator
  if(myName == "BASE") {
    pinMode(redLed, OUTPUT);
    pinMode(yellowLed, OUTPUT);
    pinMode(greenLed, OUTPUT);

    //Turn on all lights first to signify that connections are safely grounded
    digitalWrite(redLed, HIGH);
    digitalWrite(yellowLed, HIGH);
    digitalWrite(greenLed, HIGH);

    delay(1000);

    //For base station, no LED should turn on
    digitalWrite(redLed, LOW);
    digitalWrite(yellowLed, LOW);
    digitalWrite(greenLed, LOW);
  }

  HC12.listen();

  Serial.print(myName);
  Serial.println("v16 has initialized.");
  startTime = millis();
}

void loop() {
  if(myName == "BASE") {
    forBaseStation();
  } else {
    forDrone();
  }
}

void forBaseStation() {
  // State 1: Check if you received anything from base station
  if(hasReceivedMessage()) {
    Serial.print("Received '");
    Serial.print(receivedCommand);
    Serial.print("' from ");
    Serial.println(receivedFromName);

    // Interpret command
  }

  // State 2: Check if user is sending anything to base station
  if(hasSentSerially()) {
    Serial.print("Sent '");
    Serial.print(sentCommand);
    Serial.print("' to ");
    Serial.println(sentToName);

    // send command    
  }
}

void forDrone() {

}

///////General functions/////////
bool hasReceivedMessage() {
  while(HC12.available()) {
    char letter = HC12.read();

    if(letter == '\n') {
      int endIndex = receivedMessage.indexOf(' ');
      receivedCommand = receivedMessage.substring(0, endIndex);
      receivedMessage = receivedMessage.substring(endIndex+1);

      endIndex = receivedMessage.indexOf(' ');
      receivedToName = receivedMessage.substring(0, endIndex);
      receivedMessage = receivedMessage.substring(endIndex+1);

      endIndex = receivedMessage.indexOf(' ');
      receivedFromName = receivedMessage.substring(0, endIndex);
      receivedDetails = receivedMessage.substring(endIndex+1);

      receivedMessage = ""; // Erase old message
      return (receivedCommand != "") && (receivedToName == myName || receivedToName == "ALL") && (receivedFromName != "") && (receivedDetails != "");
    } else {
      receivedMessage += letter;
    }
  }

  return false;
}

void sendCommand(String command, String toName, String details) {
  //COMMAND TONAME FROMNAME DETAILS
  if(command != "" && toName != "" && details != "") {
    sentMessage = command + " " + toName + " " + myName + " " + details;
    HC12.println(sentMessage);
  } else {
    Serial.println("Wrong format of command. Try again.");
  }
}

bool hasReceivedCommand(String command) {
  return hasReceivedMessage() && (receivedCommand == command);
}

bool hasSentSerially() {
  while(Serial.available()) {
    char letter = Serial.read();

    if(letter == '\n') {
      int endIndex = sentMessage.indexOf(' ');
      sentCommand = sentMessage.substring(0, endIndex);
      sentMessage = sentMessage.substring(endIndex+1);

      endIndex = sentMessage.indexOf(' ');
      sentToName = sentMessage.substring(0, endIndex);
      sentDetails = sentMessage.substring(endIndex+1);

      sentMessage = ""; // Erase old message
      return (sentCommand != "") && (sentToName != "") && (sentDetails != "");
    } else {
      sentMessage += letter;
    }
  }

  return false;
}
