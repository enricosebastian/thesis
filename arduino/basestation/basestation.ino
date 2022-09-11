#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <LinkedList.h>

SoftwareSerial HC12(6, 7); // HC-12 TX Pin, HC-12 RX Pin

StaticJsonDocument<200> sent;
StaticJsonDocument<200> received;

LinkedList<String> myDrones;

String command = "";
String message = "";
String owner = "";

bool readCommand() {
  DeserializationError err = deserializeJson(received, HC12);
  if(err == DeserializationError::Ok) {
    command = received["COMMAND"].as<String>();
    message = received["MESSAGE"].as<String>();
    owner = received["OWNER"].as<String>();
    return true;
  } else {
    Serial.print(err.c_str());
    while(HC12.available() > 0) {
      HC12.read();
    }
  }
  return false;
}

bool hasReceivedMessage() {
  if(HC12.available()) {
    return true;
  }
  return false;
}

void setup() {
  Serial.begin(9600);
  HC12.begin(9600); 
  
  pinMode(10, OUTPUT); // DRONE 1
  pinMode(11, OUTPUT); // DRONE 2
  pinMode(12, OUTPUT); // DRONE 3

  pinMode(8, INPUT);
  pinMode(9, INPUT);
}

void loop() {
  if(hasReceivedMessage()) {
    if(readCommand()) {
      if(owner == "Jesse") {
        digitalWrite(10, HIGH);
        delay(1000);
      }
    }
  }
  digitalWrite(10, LOW);
}
