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

// void loop(void) 
// {
//   /* Get a new sensor event */ 
//   sensors_event_t event; 
//   mag.getEvent(&event);

//   float heading = atan2(event.magnetic.y, event.magnetic.x);

//   // Correct for when signs are reversed.
//   if(heading < 0)
//     heading += 2*PI;
    
//   // Check for wrap due to addition of declination.
//   if(heading > 2*PI)
//     heading -= 2*PI;
   
//   // Convert radians to degrees for readability.
//   float currentAngle = heading * 180/M_PI; 
//   float savedAngle = 90;

//   float oppositeAngle = savedAngle + 180;

//   // Case where it goes over 360-degrees
//   if(oppositeAngle > 360) {
//     oppositeAngle = oppositeAngle - 360;
//   }
//   Serial.print("Heading (degrees): "); Serial.println(currentAngle);
//   if((0 < currentAngle < savedAngle) || ((oppositeAngle < currentAngle < 360) && (currentAngle < savedAngle))) {
//     digitalWrite(greenLed, HIGH);
//     digitalWrite(yellowLed, LOW);
//     digitalWrite(blueLed, LOW);
//     digitalWrite(redLed, LOW);

//     Serial.println("right++");
//   } else if(savedAngle < currentAngle) {
//     digitalWrite(greenLed, LOW);
//     digitalWrite(yellowLed, LOW);
//     digitalWrite(blueLed, LOW);
//     digitalWrite(redLed, HIGH);

//     Serial.println("left++");
//   }
// }