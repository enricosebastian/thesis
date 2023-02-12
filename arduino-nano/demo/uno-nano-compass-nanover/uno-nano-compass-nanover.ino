#include <NeoSWSerial.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

//Constants (buttons)
const int txHc12 = 2; //green tx
const int rxHc12 = 3; //blue received

String message = "";

float pastHeading = 0.00;

NeoSWSerial HC12(txHc12, rxHc12); // (Green TX, Blue RX)
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);
  HC12.listen();
  Serial.println("Initializing nano...");

  if(!mag.begin())
  {
    /* There was a problem detecting the HMC5883 ... check your connections */
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while(1);
  }
}

void loop() {

  if(HC12.available()) {
    char letter = HC12.read();
    if(letter == '\n') {
      message += '\n';
      Serial.println(message);
      message = "";
    } else {
      message += letter;
    }
  }

  sensors_event_t event; 
  mag.getEvent(&event);
  float heading = atan2(event.magnetic.y, event.magnetic.x);

  if(heading != pastHeading) {
    Serial.println(heading-pastHeading);
    HC12.println(heading-pastHeading); 
    pastHeading = heading;
  } else {
    Serial.println(0);
    HC12.println(0); 
  }
  
}

float toDegrees(float heading) {
  float declinationAngle = 0.0349066; //https://www.magnetic-declination.com/
  heading += declinationAngle;
  if(heading < 0)
    heading += 2*PI;

  if(heading > 2*PI)
    heading -= 2*PI;

  return heading * 180/M_PI;
}