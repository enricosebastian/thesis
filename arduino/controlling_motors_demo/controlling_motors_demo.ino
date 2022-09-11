#include <Servo.h>
#include <ArduinoJson.h>

Servo ESC;

int HEIGHT = 480;
int WIDTH = 640;
int CENTER_WIDTH = WIDTH/2;
int CENTER_HEIGHT = HEIGHT/2;

void setup() {
  Serial.begin(9600);
  ESC.attach(9,1000,2000);
  ESC.write(0);
  delay(1000);
}

void loop() {
  if(Serial.available()) {
    StaticJsonDocument<300> doc;
    DeserializationError err = deserializeJson(doc, Serial);

    if(err == DeserializationError::Ok) {
      bool isFound = doc["is_found"].as<bool>();
      if(isFound) {
        ESC.write(15);
      } else {
        ESC.write(0);
      }
    }
  }
}
