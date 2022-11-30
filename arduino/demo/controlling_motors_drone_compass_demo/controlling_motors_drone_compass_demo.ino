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
int allowance = 10;
int escThreshold = 20;
float mainHeading = 0;

void setup() {
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
}

void loop() {
  if(HC12.available()) {
    DeserializationError err = deserializeJson(doc, HC12);

    if(err == DeserializationError::Ok) {
      
      String command = doc["command"].as<String>();
      int motorSpeedRight = doc["motorSpeedRight"].as<int>();
      int motorSpeedLeft = doc["motorSpeedLeft"].as<int>();
      
      if(command == "GO") {
        mainHeading = Compass.GetHeadingDegrees();
        Serial.print("Left: ");
        Serial.println(motorSpeedLeft);
  
        Serial.print("Right: ");
        Serial.println(motorSpeedRight);
        
        while(true) {
          float heading = Compass.GetHeadingDegrees();
          Serial.print("Heading: \t");
          Serial.println( heading );

          if(heading-allowance < mainHeading) {
            Serial.print("Too much left...");
            if(motorSpeedLeft < 0 || motorSpeedLeft < escThreshold) {
              motorSpeedLeft += 1;
            } else if(motorSpeedLeft >= escThreshold) {
              motorSpeedRight -= 1;
            }
          } else if (heading+allowance < mainHeading) {
            Serial.print("Too much right...");
            if(motorSpeedRight < 0 || motorSpeedRight < escThreshold) {
              motorSpeedRight += 1;
            } else if(motorSpeedRight >= escThreshold) {
              motorSpeedLeft -= 1;
            }
          }
          
          escLeft.write(motorSpeedLeft);
          escRight.write(motorSpeedRight);
        }
      }
    }
  }
}
