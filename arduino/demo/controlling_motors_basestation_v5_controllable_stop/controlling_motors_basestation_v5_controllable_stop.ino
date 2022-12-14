#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <LinkedList.h>

SoftwareSerial HC12(6, 7); // HC-12 TX Pin, HC-12 RX Pin

StaticJsonDocument<200> sent;
StaticJsonDocument<200> received;

String input;

int index = 0;
int motorSpeedLeft;
int motorSpeedRight;

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);
  Serial.println("System initializing...\n{Left motor, Right motor}:");
}

void loop() {
  if(Serial.available()) {
    input = Serial.readStringUntil('\n');
    index = input.indexOf(' ');
    
    motorSpeedLeft = input.substring(0, index).toInt();
    motorSpeedRight =input.substring(index+1).toInt();

    if(motorSpeedLeft > 180) motorSpeedLeft = 180;
    if(motorSpeedLeft < 0) motorSpeedLeft = 0;
    if(motorSpeedRight > 180) motorSpeedRight = 180;
    if(motorSpeedRight < 0) motorSpeedRight = 0;

    Serial.print("{");
    Serial.print(motorSpeedLeft);
    Serial.print(", ");
    Serial.print(motorSpeedRight);
    Serial.println("}");
    Serial.println("\n{Left motor, Right motor}:");
    if(motorSpeedLeft == 0 && motorSpeedRight == 0) {
      sent["command"] = "STOP";
    } else {
      sent["command"] = "GO";
    }
    sent["motorSpeedLeft"] = motorSpeedLeft;
    sent["motorSpeedRight"] = motorSpeedRight;
    sent["duration"] = 1;6 
    sent["controllerDelay"] = 500;
    serializeJson(sent, HC12);
  }

  if(HC12.available()) {
    DeserializationError err = deserializeJson(received, HC12);
    if(err == DeserializationError::Ok) {
      int motorSpeedLeftReceived = received["motorSpeedLeft"].as<int>();
      int motorSpeedRightReceived = received["motorSpeedRight"].as<int>();
      float mainHeadingReceived = received["mainHeading"].as<float>();
      float headingReceived = received["heading"].as<float>();
      String commentReceived = received["comment"].as<String>();

      Serial.print("Received:    ");
      Serial.print(motorSpeedLeftReceived);
      Serial.print(" - ");
      Serial.print(motorSpeedRightReceived);
      Serial.print(" - ");
      Serial.print(commentReceived);
      Serial.print(" - ");
      Serial.print(mainHeadingReceived);
      Serial.print(" - ");
      Serial.println(headingReceived);
    }
  }
}
