#include <Servo.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

SoftwareSerial HC12(6, 7); // HC-12 TX Pin, HC-12 RX Pin
Servo ESC_L;
StaticJsonDocument<300> doc;

int speedVal = 10;
int speedL = speedVal;

void setup() {
  Serial.begin(9600);
  ESC_L.attach(10,1000,2000); // (white pin, start range, end range)
}

void loop() {
  ESC_L.write(speedL);
  delay(2000);
  ESC_L.write(0); //change to 10
  delay(2000);
}
