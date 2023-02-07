#include <Servo.h>
#include <Wire.h>
#include <HMC5883L_Simple.h>
#include <NeoSWSerial.h>

HMC5883L_Simple Compass;
/*
 * GY-273 Compass Module  ->  Arduino
 * VCC  -> VCC
 * GND  -> GND
 * SCL  -> A5, blue
 * SDA  -> A4, green
*/

//Name here
// const String myName = "DRO1";
const String myName = "DRO2";
// const String myName = "DRO3";

//Constants (buttons)
const int escLeftPin = 6;
const int escRightPin = 5;
const int txNano = 9; //green tx
const int rxNano = 8; //blue received
const int waitingTime = 5000;

//movement constants
const float minSpeed = 7;
const float movingSpeed = 15;
const float maxSpeed = 20;
const float maxAngleChange = 5;

//Booleans for logic
bool hasStopped = true;
bool hasDetectedObject = false;
bool isLeft = false;

//Variables
int posX = 0;
int posY = 0;

float initialAngle = 0;
float kp = 8;
float ki = 0.2;
float kd = 30;
float PID_p, PID_i, PID_d, PID_total;

//millis time variables for storage
unsigned long startTime = 0;

//received message
String receivedMessage = "";
String receivedCommand = "";
String receivedToName = "";
String receivedFromName = "";
String receivedDetails = "";

Servo escLeft;
Servo escRight;

NeoSWSerial Nano(txNano, rxNano); // (Green TX, Blue RX)

void setup() {
  Serial.begin(9600);
  Nano.begin(9600);

  Nano.listen();

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
  Serial.println(" - Nano has initialized.");
}

void loop() {


  // Task 1: Continue to check if you have commands
  if(receiveCommand()) {
    if(receivedCommand == "GO") {
      Serial.print(myName);
      Serial.println(" is now moving.");
      initialAngle = Compass.GetHeadingDegrees();
      hasStopped = false;
      startTime = mills();
    } else if(receivedCommand == "STOP") {
      Serial.print(myName);
      Serial.println(" has stopped.");
      hasStopped = true;
      escLeft.write(0);
      escRight.write(0);
    } else if(receivedCommand == "TURN") {
      initialAngle = initialAngle + receivedDetails.toFloat();
      Serial.print("New angle: ");
      Serial.println(initialAngle);
    }
  }

  // State 1: Move drone normally
  if(!hasStopped && !hasDetectedObject) {
    move();
    if(millis() - startTime > 20000) {
      initialAngle = initialAngle + 45.00;
      startTime = millis();
    }
  }

  // State 2: You have detected something

  // State 3: You've stopped
  if(hasStopped && !hasDetectedObject) {
    //do nothing lmao
  }

  // State 4: You're going home

}

///////Specific functions/////////
void move() {
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
  Serial.print("Speed: ");
  Serial.println(modifiedSpeed);
  Serial.print("Error: ");
  Serial.println(error);

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

void sendCommand(String command, String toName, String details) {
  //COMMAND TONAME FROMNAME DETAILS
  if(command != "" && toName != "" && details != "") {
    String sentMessage = command + " " + toName + " " + myName + " " + details;
    Nano.println(sentMessage);
  } else {
    Serial.println("Wrong format of command. Try again.");
  }
}

bool receivedSpecificCommand(String command) {
  return receiveCommand() && (receivedCommand == command);
}