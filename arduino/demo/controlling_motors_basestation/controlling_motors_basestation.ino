#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <LinkedList.h>

SoftwareSerial HC12(6, 7); // HC-12 TX Pin, HC-12 RX Pin

StaticJsonDocument<200> sent;

void setup() {
  Serial.begin(9600);
  HC12.begin(9600); 
  
  pinMode(10, INPUT); // LEFT
  pinMode(11, INPUT); // RIGHT
}

void loop() {
  if(digitalRead(10) == HIGH && digitalRead(11) == HIGH) {
    sent["motor"] = "both";
    serializeJson(sent, HC12);
  } else if (digitalRead(10) == HIGH && digitalRead(11) == LOW) {
    // do left
    sent["motor"] = "left";
    serializeJson(sent, HC12);
  } else if (digitalRead(10) == LOW && digitalRead(11) == HIGH) {
    // do right
    sent["motor"] = "right";
    serializeJson(sent, HC12);
  }
}
