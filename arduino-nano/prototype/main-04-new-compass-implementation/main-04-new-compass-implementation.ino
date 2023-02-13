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

float halfX = 0.0;
float halfY = 0.0;

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
  Serial.println(" - Nano has initialized.");

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

  // Task 1: Continue to check if you have commands
  if(receiveCommand()) {
    if(receivedCommand == "X") {
      halfX = headingX;
      Serial.print("halfX: ");
      Serial.println(halfX);
    } else if(receivedCommand == "Y") {
      halfY = headingY;
      Serial.print("halfY: ");
      Serial.println(halfY);
    } else if(receivedCommand == "N") {
      headingX_N = headingX;
      headingY_N = headingY;
    } else if(receivedCommand == "NW") {
      headingX_NW = headingX;
      headingY_NW = headingY;
    } else if(receivedCommand == "W") {
      headingX_W = headingX;
      headingY_W = headingY;
    } else if(receivedCommand == "SW") {
      headingX_SW = headingX;
      headingY_SW = headingY;
    } else if(receivedCommand == "S") {
      headingX_S = headingX;
      headingY_S = headingY;
    } else if(receivedCommand == "SE") {
      headingX_SE = headingX;
      headingY_SE = headingY;
    } else if(receivedCommand == "E") {
      headingX_E = headingX;
      headingY_E = headingY;
    } else if(receivedCommand == "NE") {
      headingX_NE = headingX;
      headingY_NE = headingY;
    } else if(receivedCommand == "GO") {
      Serial.print(myName);
      Serial.println(" is now moving.");
      savedX = headingX;
      savedY = headingY;
      savedDirection = getDirection(savedX, savedY);
      hasStopped = false;
      startTime = millis();
    } else if(receivedCommand == "STOP") {
      Serial.print(myName);
      Serial.println(" has stopped.");
      hasStopped = true;
      escLeft.write(0);
      escRight.write(0);
    } else if(receivedCommand == "TURN") {
      turnTo(strDirection(receivedDetails.toInt()));
    } else if(receivedCommand == "DETE") {
      hasDetectedObject = true;
      if(receivedDetails == "CENTER") {
        turnTo(strDirection(savedDirection));
      } else if(receivedDetails == "DONE") {
        hasDetectedObject = false;
      } else {
        turnTo(receivedDetails);
      }
    }
  }

  // State 1: Move drone normally
  if(!hasStopped && !hasDetectedObject) {
    if(millis() - startTime > turnDelay) {
      if(savedDirection == 0) {
        turnTo("S");
      } else if(savedDirection == 1) {
        turnTo("SE");
      } else if(savedDirection == 2) {
        turnTo("E");
      } else if(savedDirection == 3) {
        turnTo("NE");
      } else if(savedDirection == 4) {
        turnTo("N");
      } else if(savedDirection == -3) {
        turnTo("NW");
      } else if(savedDirection == -2) {
        turnTo("W");
      } else if(savedDirection == -1) {
        turnTo("SW");
      }
    }

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
    Serial.print(headingX);
    Serial.print(", ");
    Serial.println(headingY);
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
  // Serial.print("PID_total: ");
  // Serial.println(abs(PID_total));

  int direction = getDirection(headingX, headingY);
  float allowance = 5;
  bool isStraight = (direction == savedDirection) && (abs(headingX-savedX) < allowance) && (abs(headingY-savedY) < allowance);
    
  Serial.print("\t\t\t");
  Serial.println(strDirection(direction));
  if(!isStraight) {
    if(direction < savedDirection) {
      Serial.println("right++");
    } else if(direction > savedDirection) {
      Serial.println("left++");
    } else {
      Serial.println("is straight");
    }
  } else {
    Serial.println("is straight");
  }
}

int getDirection(float headingX, float headingY) {
  float allowance = 10;
  if(headingX <= halfX && abs(headingY-halfY) < allowance) {
    // Serial.println("North");
    return 0;
  } else if(headingX <= halfX && headingY >= halfY) {
    // Serial.println("North west");
    return 1;
  } else if(abs(headingX-halfX) < allowance && headingY >= halfY) {
    // Serial.println("West");
    return 2;
  } else if(headingX >= halfX && headingY >= halfY) {
    // Serial.println("South west");
    return 3;
  } else if(headingX >= halfX && abs(headingY-halfY) < allowance) {
    // Serial.println("South");
    return 4;
  } else if(headingX >= halfX && headingY <= halfY) {
    // Serial.println("South east");
    return -3;
  } else if(abs(headingX-halfX) < allowance && headingY <= halfY) {
    // Serial.println("East");
    return -2;
  } else if(headingX <= halfX && headingY <= halfY) {
    // Serial.println("North east");
    return -1;
  }

  return 0;
}

String strDirection(int direction) {
  switch(direction) {
    case 0:
      return "N";
      break;
    case 1:
      return "NW";
      break;
    case 2:
      return "W";
      break;
    case 3:
      return "SW";
      break;
    case 4:
      return "S";
      break;
    case -3:
      return "SE";
      break;
    case -2:
      return "E";
      break;
    case -1:
      return "NE";
      break;
    default:
      return "";
  }
}

void turnTo(String direction) {
  if(direction == "N") {
    savedX = headingX_N;
    savedY = headingY_N;
    savedDirection = 0;
  } else if(direction == "NW") {
    savedX = headingX_NW;
    savedY = headingY_NW;
    savedDirection = 1;
  } else if(direction == "W") {
    savedX = headingX_W;
    savedY = headingY_W;
    savedDirection = 2;
  } else if(direction == "SW") {
    savedX = headingX_SW;
    savedY = headingY_SW;
    savedDirection = 3;
  } else if(direction == "S") {
    savedX = headingX_S;
    savedY = headingY_S;
    savedDirection = 4;
  } else if(direction == "SE") {
    savedX = headingX_SE;
    savedY = headingY_SE;
    savedDirection = -3;
  } else if(direction == "E") {
    savedX = headingX_E;
    savedY = headingY_E;
    savedDirection = -2;
  } else if(direction == "NE") {
    savedX = headingX_NE;
    savedY = headingY_NE;
    savedDirection = -1;
  } else if(direction == "LEFT") {
    if(savedDirection == 0) {
      turnTo("NW");
    } else if(savedDirection == 1) {
      turnTo("W");
    } else if(savedDirection == 2) {
      turnTo("SW");
    } else if(savedDirection == 3) {
      turnTo("S");
    } else if(savedDirection == 4) {
      turnTo("SE");
    } else if(savedDirection == -3) {
      turnTo("E");
    } else if(savedDirection == -2) {
      turnTo("NE");
    } else if(savedDirection == -1) {
      turnTo("N");
    }
  } else if(direction == "RIGHT") {
    if(savedDirection == 0) {
      turnTo("NE");
    } else if(savedDirection == 1) {
      turnTo("N");
    } else if(savedDirection == 2) {
      turnTo("NW");
    } else if(savedDirection == 3) {
      turnTo("W");
    } else if(savedDirection == 4) {
      turnTo("SW");
    } else if(savedDirection == -3) {
      turnTo("S");
    } else if(savedDirection == -2) {
      turnTo("SE");
    } else if(savedDirection == -1) {
      turnTo("E");
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