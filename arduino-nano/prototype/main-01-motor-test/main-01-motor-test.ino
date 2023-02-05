#include <NeoSWSerial.h>
#include <Servo.h>

//Name here
const String myName = "BASE";

//Constants (buttons)
const int escLeftPin = 6;
const int escRightPin = 5;
const int txNano = A2; //green tx
const int rxNano = A3; //blue received
const int waitingTime = 5000;

//movement constants
const float minSpeed = 7;
const float movingSpeed = 15;
const float maxSpeed = 20;
const float maxAngleChange = 5;

//Booleans for logic
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

NeoSWSerial Nano(txNano, rxNano); // (Green TX, Blue RX)
Servo escLeft;
Servo escRight;

void setup() {
  Serial.begin(9600);
  Nano.begin(9600);
}

void loop() {
  if(receiveCommand()) {
    if(receivedCommand == "MOVE") {
      //do standard moving
      float currentAngle = receivedDetails.toFloat();      
      moveDrone(currentAngle);
    } else if(receivedCommand == "DETE") {
      //do other logic for detection
    }
  }
}

///////Specific functions/////////
void moveDrone(float currentAngle) {
  float error = initialAngle - currentAngle;
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
  while(Nano.available()) {
    char letter = Nano.read();
    if(letter == '\n') {
      receivedMessage += '\n';
      Serial.print("Received: ");
      Serial.print(receivedMessage);

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