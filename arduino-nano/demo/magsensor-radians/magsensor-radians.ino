#include <NeoSWSerial.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

/* Assign a unique ID to this sensor at the same time */
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(1);
NeoSWSerial ser(2,3);

const float allowanceAngle = 0.03;

float pastAngle = 0;
float currentAngle = 0;
float savedAngle = 0;

bool isDeployed = false;
bool isFixed = false;

void setup(void) 
{
  Serial.begin(9600);
  ser.begin(9600);
  Serial.println("HMC5883 Magnetometer Test"); Serial.println("");
  
  /* Initialise the sensor */
  if(!mag.begin())
  {
    /* There was a problem detecting the HMC5883 ... check your connections */
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while(1);
  }
}

void loop(void) 
{

  sensors_event_t event; 
  mag.getEvent(&event);
  currentAngle = atan2(event.magnetic.y, event.magnetic.x);
  
  if(Serial.available()) {
    char letter = Serial.read();
    if(letter == 'g') {
      savedAngle = currentAngle;
      isDeployed = !isDeployed;
    } else if(letter == 'a') {
      savedAngle = savedAngle + 3;
    } else if(letter == 's') {
      savedAngle = savedAngle - 3;
    }
  }




  if(isDeployed) {
    Serial.print(savedAngle-currentAngle);
    if(abs(savedAngle-currentAngle) > allowanceAngle) {
      if(savedAngle - currentAngle < 0) {
        ser.println("\t\tfacing left");
      } else if(savedAngle - currentAngle > 0) {
        ser.println("\t\t\tfacing right");
      }
    } else {
      ser.println("\tstraight");
    }



    // if(savedAngle - currentAngle > allowanceAngle) {
    //   //it's left leaning
    // } else if(savedAngle - currentAngle < -allowanceAngle) {
    //   //it's right leaning
    // } else {
    //   //center
    // }
  }
}

void move() {
  if(savedAngle <= currentAngle-0.05 || savedAngle >= currentAngle+0.05) {
    Serial.println("not straight");
  } else {
    Serial.println("straight");
  }
}