#include <Servo.h>
#include <Wire.h>
#include <HMC5883L_Simple.h>

HMC5883L_Simple Compass;
/*
 * GY-273 Compass Module  ->  Arduino
 * VCC  -> VCC
 * GND  -> GND
 * SCL  -> A5, blue
 * SDA  -> A4, green
*/

//Name here
const String myName = "DRO1";
// const String myName = "DRO2";
// const String myName = "DRO3";

//Constants (buttons)
const int escLeftPin = 6;
const int escRightPin = 5;
const int waitingTime = 5000;

//movement constants
const float minSpeed = 7;
const float movingSpeed = 15;
const float maxSpeed = 20;
const float maxAngleChange = 5;

//Booleans for logic
bool isDeployed = false;
bool hasReceivedCommand = false;
bool hasStopped = false;
bool isLeft = false;
bool hasDetectedObject = false;


//Variables
int posX = 0;
int posY = 0;

float initialAngle = 0;
float kp = 8;
float ki = 0.2;
float kd = 30;
float PID_p, PID_i, PID_d, PID_total;

//received message
String receivedMessage = "";
String receivedCommand = "";
String receivedToName = "";
String receivedFromName = "";
String receivedDetails = "";

Servo escLeft;
Servo escRight;

void setup() {
  Serial.begin(9600);

  //Compass initialization
  Wire.begin();
  Compass.SetDeclination(-2, 37, 'W');  
  Compass.SetSamplingMode(COMPASS_SINGLE);
  Compass.SetScale(COMPASS_SCALE_130);
  Compass.SetOrientation(COMPASS_HORIZONTAL_X_NORTH);

  //ESC initialization
  escLeft.attach(escLeftPin,1000,2000);
  escRight.attach(escRightPin,1000,2000);
  escLeft.write(0);
  escRight.write(0);

  Serial.print(myName);
  Serial.println(" has initialized.");
}

void loop() {
  if(receiveCommand()) {
    hasReceivedCommand = true; // makes sure that nothing runs when you receive a command
  }

  // Stop what you're doing and interpret the command
  if(hasReceivedCommand) {
    if(receivedCommand == "GO") {
      //save new initial angle
      initialAngle = Compass.GetHeadingDegrees();
      isDeployed = true;
      hasStopped = false;
    } else if(receivedCommand == "STOP") {
      //send esc speed to zero
      escRight.write(0);
      escLeft.write(0);
      hasStopped = true;
    } else if(receivedCommand == "TURN") {
      //save new angle + toFloat();
      initialAngle = initialAngle + receivedDetails.toFloat();
    } else if(receivedCommand == "DETE") {
      //detect object
      hasDetectedObject = true;
    }
    hasReceivedCommand = false; //reset
  }


  //If you have not received any commands, and are not stopping nor detected any objects, then keep moving
  if(!hasReceivedCommand && isDeployed && !hasStopped && !hasDetectedObject) {
    //continue moving
    moveDrone();
  } else if (!hasReceivedCommand && isDeployed && hasStopped && !hasDetectedObject) {
    //do nothing lmao
  } else if (!hasReceivedCommand && isDeployed && !hasStopped && hasDetectedObject) {
    //detected object lmao
  }
}

///////Specific functions/////////
void moveDrone() {
  float error = initialAngle - Compass.GetHeadingDegrees();
  float previous_error;
  float cumulative_error;
  int period = 50;

  float PID_p = kp * error;
  float PID_i = cumulative_error * ki;
  float PID_d = kd*(error - previous_error);

  double PID_total = PID_p + PID_i + PID_d;

  cumulative_error += error;
  previous_error = error;

  float modifiedSpeed = map(abs(PID_total),0.00,1700.00,minSpeed,maxSpeed);
  
  if(error < -maxAngleChange) {
    //It's turning right, so give the right motor more speed
    // Serial.println("right");
    isLeft = false;
    escLeft.write(minSpeed);
    escRight.write(modifiedSpeed);
  } else if(error > maxAngleChange) {
    //It's turning left, so give the left motor more speed
    // Serial.println("left");
    isLeft = true;
    if(myName == "DRO1") {
      escLeft.write(modifiedSpeed+12);
      escRight.write(minSpeed);
    } else if(myName == "DRO2") {
      escLeft.write(modifiedSpeed+10);
      escRight.write(minSpeed);
    } else if(myName == "DRO3") {
      escLeft.write(modifiedSpeed+12);
      escRight.write(minSpeed);
    }
  } else {
    if(isLeft) {
      if(myName == "DRO1") {
        escLeft.write(modifiedSpeed+12);
        escRight.write(movingSpeed+2);
      } else if(myName == "DRO2") {
        escLeft.write(modifiedSpeed+10);
        escRight.write(movingSpeed);
      } else if(myName == "DRO3") {
        escLeft.write(modifiedSpeed+12);
        escRight.write(movingSpeed);
      }
    } else if(!isLeft) {
      if(myName == "DRO1") {
        escLeft.write(movingSpeed+12);
        escRight.write(modifiedSpeed);
      } else if(myName == "DRO2") {
        escLeft.write(movingSpeed+10);
        escRight.write(modifiedSpeed);
      } else if(myName == "DRO3") {
        escLeft.write(movingSpeed+12);
        escRight.write(modifiedSpeed);
      }
    }
  }
}

///////General functions/////////
bool receiveCommand() {
  while(Serial.available()) {
    char letter = Serial.read();
    if(letter == '\n') {
      receivedMessage += '\n';

      int endIndex = receivedMessage.indexOf(' ');
      receivedCommand = receivedMessage.substring(0, endIndex);
      receivedMessage = receivedMessage.substring(endIndex+1);

      endIndex = receivedMessage.indexOf(' ');
      receivedToName = receivedMessage.substring(0, endIndex);
      receivedMessage = receivedMessage.substring(endIndex+1);

      endIndex = receivedMessage.indexOf(' ');
      receivedFromName = receivedMessage.substring(0, endIndex);
      receivedDetails = receivedMessage.substring(endIndex+1);

      receivedMessage = ""; // Erase old message
      return (receivedCommand != "") && (receivedToName == myName) && (receivedFromName != "") && (receivedDetails != "");
    } else {
      receivedMessage += letter;
    }
  }
  return false;
}