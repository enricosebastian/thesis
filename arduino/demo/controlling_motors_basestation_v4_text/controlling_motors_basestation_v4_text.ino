#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <LinkedList.h>

SoftwareSerial HC12(6, 7); // HC-12 TX Pin, HC-12 RX Pin

StaticJsonDocument<200> sent;

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
    sent["command"] = "GO";
    sent["motorSpeedLeft"] = motorSpeedLeft;
    sent["motorSpeedRight"] = motorSpeedRight;
    serializeJson(sent, HC12);
  }
}
