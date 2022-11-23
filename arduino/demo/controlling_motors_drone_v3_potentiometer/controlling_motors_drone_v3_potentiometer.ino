#include <Servo.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

SoftwareSerial HC12(6, 7); // HC-12 TX Pin, HC-12 RX Pin

Servo escRight;
Servo escLeft;
StaticJsonDocument<300> doc;

int motorSpeedLeft = 0;
int motorSpeedRight = 0;

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);

  escLeft.attach(10,1000,2000);
  escRight.attach(11,1000,2000);
  
  escLeft.write(0);
  escRight.write(0);
  delay(2000);
}

void loop() {
  if(HC12.available()) {
    DeserializationError err = deserializeJson(doc, HC12);

    if(err == DeserializationError::Ok) {
      int motorSpeedRight = doc["motorSpeedRight"].as<int>();
      int motorSpeedLeft = doc["motorSpeedLeft"].as<int>();
      
      Serial.print("Left: ");
      Serial.println(motorSpeedLeft);
  
      Serial.print("Right: ");
      Serial.println(motorSpeedRight);

      escLeft.write(motorSpeedLeft);
      escRight.write(motorSpeedRight);
    }
  }
}
