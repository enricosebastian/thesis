#include <ArduinoJson.h>

const int redLed = 13;
const int yellowLed = 12;
const int greenLed = 11;

void setup() {
  Serial.begin(9600);
  
  pinMode(redLed, OUTPUT);
  digitalWrite(redLed, HIGH);
  
  pinMode(yellowLed, OUTPUT);
  digitalWrite(yellowLed, HIGH);
  
  pinMode(greenLed, OUTPUT);
  digitalWrite(greenLed, HIGH);

  delay(500);
  
  digitalWrite(redLed, LOW);
  digitalWrite(yellowLed, HIGH);
  digitalWrite(greenLed, LOW);
}

void loop() {
  if(Serial.available()) {
    StaticJsonDocument<300> received;
    DeserializationError err = deserializeJson(received, Serial);

    if(err == DeserializationError::Ok) {
      if(received["details"].as<String>() == "GREE") {
        digitalWrite(redLed, LOW);
        digitalWrite(yellowLed, LOW);
        digitalWrite(greenLed, HIGH);
      } else if(received["details"].as<String>() == "RED") {
        digitalWrite(redLed, HIGH);
        digitalWrite(yellowLed, LOW);
        digitalWrite(greenLed, LOW);
      } else if(received["details"].as<String>() == "YELL") {
        digitalWrite(redLed, LOW);
        digitalWrite(yellowLed, HIGH);
        digitalWrite(greenLed, LOW);
      } 
    }
      
  }
  Serial.println("Hello");
}
