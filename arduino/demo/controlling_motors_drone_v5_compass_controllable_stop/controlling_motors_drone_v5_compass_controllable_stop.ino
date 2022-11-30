#include <Servo.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <Arduino.h>
#include <Wire.h>
#include <HMC5883L_Simple.h>

HMC5883L_Simple Compass;
/*
 * GY-273 Compass Module  ->  Arduino
 * VCC  -> VCC  (See Note Below)
 * GND  -> GND
 * SCL  -> A5/SCL, (Use Pin 21 on the Arduino Mega)
 * SDA  -> A4/SDA, (Use Pin 20 on the Arduino Mega)
 * DRDY -> Not Connected (in this example)
 */

SoftwareSerial HC12(6, 7); // HC-12 TX Pin, HC-12 RX Pin

Servo escRight;
Servo escLeft;
StaticJsonDocument<300> doc;

int motorSpeedLeft = 0;
int motorSpeedRight = 0;
int allowance = 5;
int escThreshold = 20;
int controllerDelay = 0;
float mainHeading = 0;
String currentCommand = "";
unsigned long duration = 0;

void setup() {
  Serial.println("start...");
  Serial.begin(9600);
  HC12.begin(9600);
  Wire.begin(9600);

  Compass.SetDeclination(-2, 37, 'W');
  Compass.SetSamplingMode(COMPASS_SINGLE);
  Compass.SetScale(COMPASS_SCALE_130);
  Compass.SetOrientation(COMPASS_HORIZONTAL_X_NORTH);

  escLeft.attach(10,1000,2000);
  escRight.attach(11,1000,2000);
  
  escLeft.write(0);
  escRight.write(0);
  delay(2000);
  Serial.println("Initialization success...");
}

void loop() {
  if(HC12.available()) {
    DeserializationError err = deserializeJson(doc, HC12);
    
    if(err == DeserializationError::Ok) {
      String receivedCommand = doc["command"].as<String>();
      Serial.print("COMMAND RECEIVED: ");
      Serial.println(receivedCommand);
      
      if(receivedCommand == "GO") {
        currentCommand = "GO";
        motorSpeedRight = doc["motorSpeedRight"].as<int>();
        motorSpeedLeft = doc["motorSpeedLeft"].as<int>();
        duration = doc["duration"].as<unsigned long>();
        controllerDelay = doc["controllerDelay"].as<int>();
        
        mainHeading = Compass.GetHeadingDegrees();
        Serial.print("Main heading: ");
        Serial.println(mainHeading);
        
        Serial.print("Left: ");
        Serial.println(motorSpeedLeft);
  
        Serial.print("Right: ");
        Serial.println(motorSpeedRight);
      } else if(receivedCommand == "STOP") {
        currentCommand = "STOP";
        motorSpeedLeft = 0;
        motorSpeedRight = 0;
      } else {
        currentCommand = "";
      }
    }
  }

  if(currentCommand == "GO") {
    float heading = Compass.GetHeadingDegrees();
    Serial.print("Original heading: ");
    Serial.println(mainHeading);
    Serial.print("Heading: \t");
    Serial.println( heading );
    Serial.println("\n");

    if(heading < mainHeading-allowance) {
      Serial.println("Too much left...");
      if(motorSpeedLeft < 0 || motorSpeedLeft < escThreshold) {
        motorSpeedLeft += 1;
      } else if(motorSpeedLeft >= escThreshold) {
        motorSpeedRight -= 1;
      }
    } else if (heading > mainHeading+allowance) {
      Serial.println("Too much right...");
      if(motorSpeedRight < 0 || motorSpeedRight < escThreshold) {
        motorSpeedRight += 1;
      } else if(motorSpeedRight >= escThreshold) {
        motorSpeedLeft -= 1;
      }
    } else {
      Serial.println("All good...");
    }
    
    Serial.print("{");
    Serial.print(motorSpeedLeft);
    Serial.print(", ");
    Serial.print(motorSpeedRight);
    Serial.println("}");
    Serial.println("\n");
    escLeft.write(motorSpeedLeft);
    escRight.write(motorSpeedRight);
    delay(controllerDelay);
  } else if(currentCommand == "STOP") {
    Serial.println("Stopped running...");
    escLeft.write(0);
    escRight.write(0);
  } else {
    Serial.println("Null command...");
    escLeft.write(0);
    escRight.write(0);
  }
}
