#include <Servo.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

SoftwareSerial HC12(6, 7); // HC-12 TX Pin, HC-12 RX Pin

Servo ESC_R;
Servo ESC_L;
StaticJsonDocument<300> doc;

int speedR = 0;
int speedL = 0;

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);

  ESC_L.attach(10,1000,2000);
  ESC_R.attach(11,1000,2000);
  
  ESC_L.write(0);
  ESC_R.write(0);
  delay(2000);
}

void loop() {
  if(HC12.available()) {
    DeserializationError err = deserializeJson(doc, HC12);

    if(err == DeserializationError::Ok) {
      String motor = doc["motor"].as<String>();
      int motorSpeed = doc["speed"].as<int>();
      Serial.println(motor);
      if (motor == "left") {
        speedL = motorSpeed;
      } else if (motor == "right") {
        speedR = motorSpeed;
      } else if (motor == "both") {
        speedL = motorSpeed;
        speedR = motorSpeed;
      }
      Serial.print("Left: ");
      Serial.println(speedL);
      Serial.print("Right: ");
      Serial.println(speedR);
      ESC_L.write(speedL);
      ESC_R.write(speedR);
    }
  }
}
