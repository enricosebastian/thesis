#include <NeoSWSerial.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <Servo.h>

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
const int turnDelay = 30000; //in milliseconds

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
int savedDirection = 0;

float initialAngle = 0;
float initialStraightAngle = 0;
float kp = 8; //5
float ki = 0.2;
float kd = 30;
float PID_p, PID_i, PID_d, PID_total;

float maxX = 0;
float maxY = 0;
float minX = 1000;
float minY = 1000; 

float savedX = 0;
float savedY = 0;

float headingX_N = 0;
float headingX_E = 0;
float headingX_W = 0;
float headingX_S = 0;

float headingX_NE = 0;
float headingX_NW = 0;
float headingX_SE = 0;
float headingX_SW = 0;

float headingY_N = 0;
float headingY_E = 0;
float headingY_W = 0;
float headingY_S = 0;

float headingY_NE = 0;
float headingY_NW = 0;
float headingY_SE = 0;
float headingY_SW = 0;

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
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

void setup() {
  Serial.begin(9600);
  Nano.begin(9600);

  Nano.listen();

  //ESC initialization
  escLeft.attach(escLeftPin,1000,2000);
  escRight.attach(escRightPin,1000,2000);
  escLeft.write(0);
  escRight.write(0);

  Serial.print(myName);
  Serial.println(" - Nano v4.0 has initialized.");

  //HMC5883 initialization
  if(!mag.begin())
  {
    /* There was a problem detecting the HMC5883 ... check your connections */
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while(1);
  }
}

void loop() {
  sensors_event_t event; 
  mag.getEvent(&event);

  if(maxY < event.magnetic.y) maxY = event.magnetic.y;
  if(minY > event.magnetic.y) minY = event.magnetic.y;

  if(maxX < event.magnetic.x) maxX = event.magnetic.x;
  if(minX > event.magnetic.x) minX = event.magnetic.x;

  float headingX = map(event.magnetic.x, minX, maxX, 0, 180);
  float headingY = map(event.magnetic.y, minY, maxY, 0, 180);

  Serial.print(headingX);
  Serial.print(", ");
  Serial.println(headingY);

  // Task 1: Continue to check if you have commands
  if(receiveCommand()) {
    if(receivedCommand == "GO") {
      Serial.print(myName);
      Serial.println(" is now moving.");
      hasStopped = false;
      startTime = millis();
    } else if(receivedCommand == "STOP") {
      Serial.print(myName);
      Serial.println(" has stopped.");
      hasStopped = true;
      escLeft.write(0);
      escRight.write(0);
    } else if(receivedCommand == "TURN") {
      //turn
    } else if(receivedCommand == "DETE") {
      hasDetectedObject = true;
      if(receivedDetails == "DONE") {
        hasDetectedObject = false;
      }
    }
  }

  // State 1: Move drone normally
  if(!hasStopped && !hasDetectedObject) {
    move(headingX, headingY);
  }

  // State 2: You have detected something
  if(!hasStopped && hasDetectedObject) {
    move(headingX, headingY);
    //don't do any movement algorithms
  }

  // State 3: You've stopped
  if(hasStopped && !hasDetectedObject) {
    //do nothing lmao
  }

  // State 4: You're going home

}

///////Specific functions/////////
void move(float headingX, float headingY) {
  float error = atan2(savedY, savedX) - atan2(headingY, headingX);
  float previous_error;
  float cumulative_error;
  int period = 50;

  float PID_p = kp * error;
  float PID_i = cumulative_error * ki;
  float PID_d = kd*(error - previous_error);

  double PID_total = PID_p + PID_i + PID_d;

  cumulative_error += error;
  previous_error = error;

  float modifiedSpeed = map(abs(PID_total),0.00,8000.00,minSpeed,maxSpeed);
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
      return (receivedCommand != "") && (receivedToName == myName || receivedToName == "ALL") && (receivedFromName != "") && (receivedDetails != "");
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