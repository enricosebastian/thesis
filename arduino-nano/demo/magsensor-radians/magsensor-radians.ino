#include <NeoSWSerial.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

const int rxPin = 2;
const int txPin = 3;

const float allowanceAngle = 0.03;

float pastAngle = 0;
float currentAngle = 0;
float savedAngle = 0;

bool isDeployed = false;
bool isFixed = false;
bool isReceiver = false;

/* Assign a unique ID to this sensor at the same time */
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(1);
NeoSWSerial ser(rxPin,txPin); //rxPin blue, txPin green

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
  if(ser.available()) {
    Serial.println("message");
  }
}
