#include <ArduinoJson.h>
#include <SoftwareSerial.h>

SoftwareSerial HC12(6, 7); // HC-12 TX Pin, HC-12 RX Pin

StaticJsonDocument<200> sent;
StaticJsonDocument<200> received;

String message = "";

StaticJsonDocument<200> doc1() {
  sent["COMMAND"] =  "command 1";
  sent["MESSAGE"] = "message 1";
  
  return sent;
}

StaticJsonDocument<200> doc2() {
  sent["COMMAND"] =  "command 2";
  sent["MESSAGE"] = "message 2";
  
  return sent;
}

void setup() {
  Serial.begin(9600);
  HC12.begin(9600); 
  
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  
  pinMode(8, INPUT);
}

void loop() {
  if(digitalRead(8) == HIGH) {
    digitalWrite(9, HIGH);
    serializeJson(doc1(), HC12);
  } else {
    digitalWrite(9, LOW);
    digitalWrite(10, LOW);
  }

  if(HC12.available()) {
    DeserializationError err = deserializeJson(received, HC12);
    if(err == DeserializationError::Ok) {
      message = received["MESSAGE"].as<String>();
      digitalWrite(10, HIGH);
      Serial.println(message);
      delay(1000);
    } else {
      Serial.print(err.c_str());
      while(HC12.available() > 0) {
        HC12.read();
      }
    }
  }
}
