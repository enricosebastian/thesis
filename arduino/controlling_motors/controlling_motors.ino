#include <Servo.h>
#include <ArduinoJson.h>

Servo ESC_R;
Servo ESC_L;

int HEIGHT = 480;
int WIDTH = 640;
int CENTER_WIDTH = WIDTH/2;
int CENTER_HEIGHT = HEIGHT/2;

int CENTER_ALLOWANCE = 100;

int speedVal = 0;
int speedR = speedVal;
int speedL = speedVal;

void setup() {
  Serial.begin(9600);
  ESC_R.attach(9,1000,2000);
  ESC_L.attach(10,1000,2000);
  
  ESC_L.write(speedL);
  ESC_R.write(speedR);
  delay(1000);
}

void loop() {
  if(Serial.available()) {
    StaticJsonDocument<300> doc;
    DeserializationError err = deserializeJson(doc, Serial);

    if(err == DeserializationError::Ok) {
      bool isFound = doc["is_found"].as<bool>();
      int origin_x = doc["origin_x"].as<int>();
      int height = doc["height"].as<int>();
      int width = doc["width"].as<int>();
      if(isFound) {
        if(height * width >= 60000) {
          speedVal = 1;
        } else if (height * width < 60000 && height*width > 30000) {
          speedVal = 5;
        } else if (height * width <= 30000) {
          speedVal = 10;
        }
        
        if(origin_x >= CENTER_WIDTH-CENTER_ALLOWANCE && origin_x <= CENTER_WIDTH+CENTER_ALLOWANCE) {
          // walk straight
          speedL = speedVal;
          speedR = speedVal;
        } else if(origin_x < CENTER_WIDTH-CENTER_ALLOWANCE) {
          speedL = 0;
          speedR = speedVal;
        } else if(origin_x > CENTER_WIDTH+CENTER_ALLOWANCE) {
          speedL = speedVal;
          speedR = 0;
        }
        
      } else {
        speedL = 0;
        speedR = 0;
      }
      ESC_L.write(speedL);
      ESC_R.write(speedR);
      
    }
  }
}
