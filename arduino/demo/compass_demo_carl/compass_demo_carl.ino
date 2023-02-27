#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

/* Assign a unique ID to this sensor at the same time 
 * GY-273 Compass Module  ->  Arduino
 * VCC  -> VCC  (See Note Below)
 * GND  -> GND
 * SCL  -> A5/SCL, (Use Pin 21 on the Arduino Mega)
 * SDA  -> A4/SDA, (Use Pin 20 on the Arduino Mega)
 * DRDY -> Not Connected (in this example)
 * 
*/

Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);
bool x = true;
void setup(void) 
{
  Serial.begin(9600);
  Serial.println("HMC5883 Magnetometer Test"); Serial.println("");
  
  /* Initialise the sensor */
  if(!mag.begin())
  {
    /* There was a problem detecting the HMC5883 ... check your connections */
    Serial.println("no HMC5883 detected");
    while(1);
  } 
}
void loop(void) 
{
  /* Saves the current position of boat*/
  float saved_position; 
  if(x)
  {
    saved_position = starter_position();
    x = false;
  }
  Serial.print("Saved Position: "); Serial.println(saved_position); 

  /*checks current position if it moved */
  float current_pos = getPosition();
  if(current_pos<saved_position-5){
    Serial.print("Current position: ");Serial.println(current_pos);
    //Serial.print("move to the right \n"); // put code here to adjust to the right
  }
  else if (current_pos>saved_position+5){
    Serial.print("Current position: ");Serial.println(current_pos);
    //Serial.print("move to the left \n"); // put code here to adjust to the left
  }
  delay(1000);
}

float starter_position(){
  float x = getPosition();
  return x;
}

float getPosition(){
  sensors_event_t event; 
  mag.getEvent(&event);
  float heading = atan2(event.magnetic.y, event.magnetic.x);
  float declinationAngle = 0.22;
  heading += declinationAngle;
  // Correct for when signs are reversed.
  if(heading < 0)
    heading += 2*PI;
  // Check for wrap due to addition of declination.
  if(heading > 2*PI)
    heading -= 2*PI;
  // Convert radians to degrees 
  float headingDegrees = heading * 180/M_PI;
  return headingDegrees;
}
