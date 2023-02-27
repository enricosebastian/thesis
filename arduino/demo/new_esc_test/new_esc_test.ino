#include <Servo.h>

Servo ESC_L;
int analogPin = A0;

void setup() {
  Serial.begin(9600);
  ESC_L.attach(10,1000,2000); // (white pin, start range, end range)
  ESC_L.write(0);
  delay(1000);
} 

void loop() {
  int val = 180.0 * analogRead(analogPin) / 1023.0;
  Serial.println(val);
  ESC_L.write(val);
  
}
