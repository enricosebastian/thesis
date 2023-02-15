#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

/* Assign a unique ID to this sensor at the same time */
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

void setup(void) 
{
  Serial.begin(9600);
  Serial.println("HMC5883 Magnetometer Test"); Serial.println("");
  
  /* Initialise the sensor */
  if(!mag.begin())
  {
    /* There was a problem detecting the HMC5883 ... check your connections */
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while(1);
  }

  for(int i=0; i<360; i++) {
    float savedAngle = 180;

    float currentAngle = i; // 91

    float oppositeAngle = savedAngle + 180; //271

    // Case where it goes over 360-degrees
    if(oppositeAngle > 360) {
      oppositeAngle = oppositeAngle - 360;
    }

    //                     0 < 350 t        && 350 < 270      f  == (f)     ||    170 < 350     t &&     350< 360   t
    bool rightStatement = ((0 < currentAngle) && (currentAngle < savedAngle)) || ((oppositeAngle < currentAngle) && (currentAngle <= 360));
    bool leftStatement = savedAngle < currentAngle;

    // Serial.print(0);
    // Serial.print("<");
    // Serial.print(currentAngle);
    // Serial.print("<");
    // Serial.print(savedAngle);
    // Serial.print(": ");
    // Serial.println(rightStatement);

    // Serial.print(oppositeAngle);
    // Serial.print("<");
    // Serial.print(currentAngle);
    // Serial.print("<=");
    // Serial.print(360);
    // Serial.print(": ");
    // Serial.println(leftStatement);

    Serial.print("Saved angle: ");
    Serial.println(savedAngle);
    Serial.print("Current angle: ");
    Serial.println(currentAngle);
    //  0 < 91 < 90    f             271 < 91 < 360 f
    if(rightStatement) {
      Serial.println("\t\tright++");
    } else if(leftStatement) {
      Serial.println("left++");
    }
  }
}

void loop() {

}