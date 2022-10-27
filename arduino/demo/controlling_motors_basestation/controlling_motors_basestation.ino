#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <LinkedList.h>

SoftwareSerial HC12(6, 7); // HC-12 TX Pin, HC-12 RX Pin

StaticJsonDocument<200> sent;

int leftBtn = 8;
int rightBtn = 9;

void setup() {
  Serial.begin(9600);
  HC12.begin(9600); 
  
  pinMode(leftBtn, INPUT); // LEFT
  pinMode(rightBtn, INPUT); // RIGHT
}

void loop() {
  if (digitalRead(leftBtn) == HIGH) {
    // do left
    sent["motor"] = "left";
    serializeJson(sent, HC12);
  } else if (digitalRead(rightBtn) == HIGH) {
    // do right
    sent["motor"] = "right";
    serializeJson(sent, HC12);
  }
  delay(200); // for debouncing
}
