
#include <Servo.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

SoftwareSerial HC12(6, 7); // HC-12 TX Pin, HC-12 RX Pin

Servo ESC_R;
Servo ESC_L;
StaticJsonDocument<300> doc;

//
int HEIGHT = 480;
int WIDTH = 640;
int CENTER_WIDTH = WIDTH/2;
int CENTER_HEIGHT = HEIGHT/2;

int CENTER_ALLOWANCE = 100;

void setup() {
  Serial.begin(9600);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
}

void loop() {
  if(Serial.available()) {
    DeserializationError err = deserializeJson(doc, Serial);

    if(err == DeserializationError::Ok) {
      String motor = doc["motor"].as<String>();
      
      if(motor == "both") {
        digitalWrite(8, HIGH);
        digitalWrite(9, HIGH);
      } else if (motor == "left") {
        digitalWrite(8, LOW);
        digitalWrite(9, HIGH);
      } else if(motor == "right") {
        digitalWrite(8, HIGH);
        digitalWrite(9, LOW);
      } else {
      digitalWrite(8, LOW);
      digitalWrite(9, LOW);
    }
  }
}
