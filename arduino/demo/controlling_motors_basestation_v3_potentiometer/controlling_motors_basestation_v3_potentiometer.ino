#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <LinkedList.h>

SoftwareSerial HC12(6, 7); // HC-12 TX Pin, HC-12 RX Pin

StaticJsonDocument<200> sent;

int leftBtn = 8;
int rightBtn = 9;
String motor;

int motorSpeedLeft = 0;
int motorSpeedRight = 0;

int motorSpeedLeftPrevious = motorSpeedLeft;
int motorSpeedRightPrevious = motorSpeedRight;

int allowance = 15;

void setup() {
  Serial.begin(9600);
  HC12.begin(9600); 
  
  pinMode(leftBtn, INPUT); // LEFT
  pinMode(rightBtn, INPUT); // RIGHT
}

void loop() {
  motorSpeedRight = analogRead(A0);
  motorSpeedLeft = analogRead(A1);

  motorSpeedRight = map(motorSpeedRight, 0, 1023, 0, 180);
  motorSpeedLeft = map(motorSpeedLeft, 0, 1023, 0, 180);
  
  if( !(motorSpeedLeftPrevious+allowance >= motorSpeedLeft) || !(motorSpeedLeftPrevious-allowance <= motorSpeedLeft)|| !(motorSpeedRightPrevious+allowance >= motorSpeedRight) || !(motorSpeedRightPrevious-allowance <= motorSpeedRight)) {
    sent["motorSpeedLeft"] = motorSpeedLeft;
    sent["motorSpeedRight"] = motorSpeedRight;
    serializeJson(sent, HC12);
    
    motorSpeedLeftPrevious = motorSpeedLeft;
    motorSpeedRightPrevious = motorSpeedRight;
    Serial.print("Left: ");
    Serial.println(motorSpeedLeftPrevious);
  
    Serial.print("Right: ");
    Serial.println(motorSpeedRightPrevious);
  }
}
