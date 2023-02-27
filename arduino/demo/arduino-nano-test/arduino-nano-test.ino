#include<Servo.h>

Servo esc;
const int escPin = 6;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  esc.attach(escPin, 1000, 2000);
  esc.write(0);
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  esc.write(11);
  delay(1000);
  esc.write(0);
  delay(1000);
}
