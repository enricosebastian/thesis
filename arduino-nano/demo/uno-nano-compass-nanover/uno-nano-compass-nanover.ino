#include <NeoSWSerial.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

//Constants (buttons)
const int txHc12 = 2; //green tx
const int rxHc12 = 3; //blue received

String message = "";

float heading = 0.01;
float pastHeading = 0.00;

float maxX = 0;
float maxY = 0;
float minX = 1000;
float minY = 1000; 

float halfX = 0.0;
float halfY = 0.0;

bool turnedRightFirst = false;
bool turned = false;

bool isDeployed = false;

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

  sensors_event_t event; 
  mag.getEvent(&event);
  heading = atan2(event.magnetic.y, event.magnetic.x);
  pastHeading = heading+0.01;
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
  heading = atan2(event.magnetic.y, event.magnetic.x);

  if(maxY < event.magnetic.y) maxY = event.magnetic.y;
  if(minY > event.magnetic.y) minY = event.magnetic.y;

  if(maxX < event.magnetic.x) maxX = event.magnetic.x;
  if(minX > event.magnetic.x) minX = event.magnetic.x;

  float headingX = map(event.magnetic.x, minX, maxX, 0, 180);
  float headingY = map(event.magnetic.y, minY, maxY, 0, 180);

  if(Serial.available()) {
    char letter = Serial.read();
    if(letter == 'y') {
      halfY = headingY;
      Serial.print("halfY: ");
      Serial.println(halfY);
    } else if(letter == 'x') {
      halfX = headingX;
      Serial.print("halfX: ");
      Serial.println(halfX);
    } else if(letter == 'g') {
      isDeployed = !isDeployed;
    }
  }

  if(!isDeployed) {
    Serial.print(headingX);
    Serial.print(", ");
    Serial.println(headingY);
  }

  if(isDeployed) {
    int direction = getDirection(headingX, headingY);
  }
}

int getDirection(float headingX, float headingY) {
  float allowance = 10;
  if(headingX <= halfX && abs(headingY-halfY) < allowance) {
    Serial.println("North");
    return 1;
  } else if(headingX <= halfX && headingY >= halfY) {
    Serial.println("North west");
    return 2;
  } else if(abs(headingX-halfX) < allowance && headingY >= halfY) {
    Serial.println("West");
    return 3;
  } else if(headingX >= halfX && headingY >= halfY) {
    Serial.println("South west");
    return 4;
  } else if(headingX >= halfX && abs(headingY-halfY) < allowance) {
    Serial.println("South");
    return 5;
  } else if(headingX >= halfX && headingY <= halfY) {
    Serial.println("South east");
    return 6;
  } else if(abs(headingX-halfX) < allowance && headingY <= halfY) {
    Serial.println("East");
    return 7;
  } else if(headingX <= halfX && headingY <= halfY) {
    Serial.println("North east");
    return 8;
  } else {
    return 0;
  }



  return 0;


  // if(headingX >= minX && headingX <= halfX) {
  //   return ("North east or west");
  // } else if(headingX <= maxX && headingX >= halfX) {
  //   return ("South east or west");
  // }
}


  // if((heading != pastHeading) && !turned) {
  //   if(heading - pastHeading < -0.05){
  //     turned = true;
  //     turnedRightFirst = true;
  //   } else if(heading - pastHeading > 0.05) {
  //     turned = true;
  //     turnedRightFirst = false;
  //   }
  //   pastHeading = heading;
  // }