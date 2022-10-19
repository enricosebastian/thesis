
#include <Servo.h>
#include <ArduinoJson.h>

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
  pinMode(10, OUTPUT);
}

void loop() {
  if(Serial.available()) {
    DeserializationError err = deserializeJson(doc, Serial);
    
    if(err == DeserializationError::Ok) {
      bool isFound = doc["is_found"].as<bool>();
      if(isFound) {
        int origin_x = doc["origin_x"].as<int>();
        if(origin_x >= CENTER_WIDTH-CENTER_ALLOWANCE && origin_x <= CENTER_WIDTH+CENTER_ALLOWANCE) {
          digitalWrite(9, HIGH);
          digitalWrite(10, HIGH);
        } else if (origin_x < CENTER_WIDTH-CENTER_ALLOWANCE) {
          digitalWrite(9, LOW);
          digitalWrite(10, HIGH);
        } else if(origin_x > CENTER_WIDTH+CENTER_ALLOWANCE) {
          digitalWrite(9, HIGH);
          digitalWrite(10, LOW);
        }
      } else {
        digitalWrite(8, LOW);
        digitalWrite(9, LOW);
        digitalWrite(10, LOW);
      }
    }
  }
}
