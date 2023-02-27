#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <LinkedList.h>

SoftwareSerial HC12(6, 7); // HC-12 TX Pin, HC-12 RX Pin

StaticJsonDocument<200> sent;

int leftBtn = 8;
int rightBtn = 9;
String motor;
int motorSpeed = 0;

void setup() {
  Serial.begin(9600);
  HC12.begin(9600); 
  
  pinMode(leftBtn, INPUT); // LEFT
  pinMode(rightBtn, INPUT); // RIGHT
  Serial.println("Send commands by typing 'cmd'");
}

void loop() {
  if(Serial.available()) {
    if(Serial.readStringUntil('\n') == "cmd") {
      Serial.println("Input motor and motor speed:");
      while(true) {
        if(Serial.available()) {
          motor = Serial.readStringUntil('\n');
          Serial.println("MOTOR: "+motor);
          while(true) {
            if(Serial.available()) {
              motorSpeed = Serial.parseInt();
              Serial.print("SPEED: ");
              Serial.println(motorSpeed);
              break;
            }
          }
          Serial.println("{"+motor+", "+motorSpeed+"}");
          sent["motor"] = motor;
          sent["speed"] = motorSpeed;
          serializeJson(sent, HC12);
          motor = "";
          motorSpeed = 0;
          Serial.println("\n\nSend commands by typing 'cmd':");
          break;
        }
      }
    }
  }
}
