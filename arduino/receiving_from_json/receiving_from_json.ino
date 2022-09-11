#include <ArduinoJson.h>

String message;
int HEIGHT = 480;
int WIDTH = 640;
int CENTER_WIDTH = WIDTH/2;
int CENTER_HEIGHT = HEIGHT/2;

void setup() {
  Serial.begin(9600);
}

void loop() {
  if(Serial.available()) {
    StaticJsonDocument<300> doc;
    DeserializationError err = deserializeJson(doc, Serial);

    if(err == DeserializationError::Ok) {
      int width = doc["width"].as<int>();
      int height = doc["height"].as<int>();
      int origin_x = doc["origin_x"].as<int>();
      int origin_y = doc["origin_y"].as<int>();
      
      if(origin_x >= CENTER_WIDTH-50 && origin_x <= CENTER_WIDTH+50) {
        //two motors
      } else if (origin_x < CENTER_WIDTH-50) {
        //right motor / rotate counterclockwise
      } else if (origin_x > CENTER_WIDTH+50) {
        //left motor / rotate clockwise
      }
    }
      
  }

  
}
