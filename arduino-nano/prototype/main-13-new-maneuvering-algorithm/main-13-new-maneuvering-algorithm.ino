#include <NeoSWSerial.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <Servo.h>

//Name here
const String myName = "DRO1";
// const String myName = "DRO2";
// const String myName = "DRO3";

//Constants
const int greenLed = 7;
const int yellowLed = 8;
const int blueLed = 9;
const int redLed = 10;
const int escLeftPin = 5;
const int escRightPin = 6;

const int rxNano = 11; 
const int txNano = 12; 

const int waitingTime = 5000;
const int turnDelay = 30000; //in milliseconds

//movement constants
const float stopSpeed = 0;
const float minSpeed = 15;
const float maxSpeed = 25;

const float angleAllowance = 5.0;
const float currentXAllowance = 2;

//Booleans for logic
bool isConnected = false;
bool isDeployed = false;
bool hasStopped = true;
bool hasDetectedObject = false;
bool isGoingHome = false;

//Variables
float d1 = 0;
float d2 = 0;

float savedX = 0;
float savedY = 0;

float currentX = 0;
float currentY = 0;

float homeX = 0;
float homeY = 0;

float maxY = 11.0;
float minY = 10.0;

//PID values
float kp = 2;
float ki = 0.2;
float kd = 3;
float PID_p, PID_i, PID_d, PID_total;

float tempAngle = 0.0;
float savedAngle = 0.0;
float straightAngle = 0.0;
float oppositeStraightAngle = 0.0;
float leftAngle = 0.0;
float rightAngle = 0.0;
float detectAngle = -90.0;

//millis time variables for storage
unsigned long startTime = 0;
unsigned long startTime2 = 0;

//received message
String receivedMessage = "";
String receivedCommand = "";
String receivedToName = "";
String receivedFromName = "";
String receivedDetails = "";

Servo escLeft;
Servo escRight;

int counter = 0;

NeoSWSerial Nano(rxNano, txNano);

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

  //HMC5883 initialization
  if(!mag.begin())
  {
    /* There was a problem detecting the HMC5883 ... check your connections */
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while(1);
  }

  //LED initialization
  pinMode(greenLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  pinMode(redLed, OUTPUT);

  digitalWrite(greenLed, HIGH);
  digitalWrite(yellowLed, HIGH);
  digitalWrite(blueLed, HIGH);
  digitalWrite(redLed, HIGH);

  delay(1000);

  digitalWrite(greenLed, LOW);
  digitalWrite(yellowLed, LOW);
  digitalWrite(blueLed, LOW);
  digitalWrite(redLed, HIGH);

  Serial.print(myName);
  Serial.println(" - v9 Nano has initialized.");
}

void loop() {
  sensors_event_t event; 
  mag.getEvent(&event);

  float heading = atan2(event.magnetic.y, event.magnetic.x);

  // Correct for when signs are reversed.
  if(heading < 0)
    heading += 2*PI;

  // Check for wrap due to addition of declination.
  if(heading > 2*PI)
    heading -= 2*PI;
   
  // Convert radians to degrees for readability.
  float currentAngle = heading * 180/M_PI; 
  
  // Main task: Continue to check for commands
  if(receiveCommand()) {
    Serial.print("Received: ");
    Serial.println(receivedCommand);

    if(receivedCommand == "CONN" && !isConnected) {
      digitalWrite(greenLed, LOW);
      digitalWrite(yellowLed, HIGH);
      digitalWrite(blueLed, LOW);
      digitalWrite(redLed, LOW);

      isConnected = true;
      Serial.println("Connected to the base station.");
    } else if(receivedCommand == "DEPL" && !isDeployed) {
      isDeployed = true;
      hasStopped = true;

      int endIndex = receivedDetails.indexOf(',');
      homeX = receivedDetails.substring(0, endIndex).toFloat();
      homeY = receivedDetails.substring(endIndex+1).toFloat();

      startTime = millis();

      Serial.print(myName);
      Serial.print(" is deployed. Home coordinates are: ");
      Serial.print(homeX);
      Serial.print(",");
      Serial.println(homeY);
    } else if(receivedCommand == "GO" && isDeployed) {
      hasStopped = false;
      hasDetectedObject = false;
      startTime = millis();
      startTime2 = millis();

      savedAngle = currentAngle;
      straightAngle = savedAngle;

      leftAngle = savedAngle + detectAngle;
      if(leftAngle > 360) leftAngle = leftAngle - 360;
      if(leftAngle < 0) leftAngle = 360 + leftAngle;

      rightAngle = savedAngle - detectAngle;
      if(rightAngle > 360) rightAngle = rightAngle - 360;
      if(rightAngle < 0) rightAngle = 360 + rightAngle;

      int endIndex = receivedDetails.indexOf(',');
      homeX = receivedDetails.substring(0, endIndex).toFloat();
      homeY = receivedDetails.substring(endIndex+1).toFloat();
      //235 = blue body/dro1/white pbank
      //225 

      if(myName == "DRO1") oppositeStraightAngle = straightAngle + 235;
      else if(myName == "DRO2") oppositeStraightAngle = straightAngle + 210;

      if(oppositeStraightAngle > 360) {
        oppositeStraightAngle = oppositeStraightAngle - 360;
      }

      digitalWrite(greenLed, HIGH);
      digitalWrite(yellowLed, LOW);
      digitalWrite(blueLed, LOW);
      digitalWrite(redLed, LOW);

      Serial.print(myName);
      Serial.print(" is now moving at ");
      Serial.print(savedAngle);
      Serial.print(" degrees in ");
      Serial.print(savedX);
      Serial.print(",");
      Serial.println(savedY);

      Serial.print("Opposite straight angle: ");
      Serial.println(oppositeStraightAngle);
    } else if(receivedCommand == "STOP" && isDeployed) {
      hasStopped = true;
      hasDetectedObject = false;
      isGoingHome = false;
      startTime = millis();

      Serial.print(myName);
      Serial.println(" has stopped.");
      
      escLeft.write(stopSpeed);
      escRight.write(stopSpeed);
    } else if(receivedCommand == "DETE" && isDeployed) {
      hasDetectedObject = true;
      startTime = millis();
      startTime2 = millis();

      Serial.println(receivedDetails == "LEFT\r");
      Serial.println(receivedDetails == "LEFT");
      Serial.println(receivedDetails == "LEFT\n");

      if(receivedDetails == "LEFT\r" || receivedDetails == "LEFT") {
        digitalWrite(greenLed, LOW);
        digitalWrite(yellowLed, LOW);
        digitalWrite(blueLed, HIGH);
        digitalWrite(redLed, LOW);

        savedAngle = rightAngle;
      } else if(receivedDetails == "RIGHT\r" || receivedDetails == "RIGHT") {
        digitalWrite(greenLed, LOW);
        digitalWrite(yellowLed, HIGH);
        digitalWrite(blueLed, LOW);
        digitalWrite(redLed, LOW);

        savedAngle = leftAngle;
      } else if(receivedDetails == "CENTER\r" || receivedDetails == "CENTER") {
        digitalWrite(greenLed, LOW);
        digitalWrite(yellowLed, HIGH);
        digitalWrite(blueLed, HIGH);
        digitalWrite(redLed, LOW);

        savedAngle = savedAngle;
      } else if(receivedDetails == "DONE\r" || receivedDetails == "DONE") {
        digitalWrite(greenLed, LOW);
        digitalWrite(yellowLed, LOW);
        digitalWrite(blueLed, LOW);
        digitalWrite(redLed, LOW);
        Serial.println("Object has been acquired.");
      }
    } else if(receivedCommand == "COOR" && isDeployed) {
      int endIndex = receivedDetails.indexOf(',');
      currentX = receivedDetails.substring(0, endIndex).toFloat();
      currentY = receivedDetails.substring(endIndex+1).toFloat();
    } else if(receivedCommand == "TURN" && isDeployed) {
      savedAngle = savedAngle + receivedDetails.toFloat();
      
      Serial.print("Turning to: ");
      Serial.println(savedAngle);
    } else if(receivedCommand == "HOME" && isDeployed) {
      hasStopped = true;
      isGoingHome = true;

    } else if(receivedCommand == "MAXY") {
      Serial.print("Old maxY: ");
      Serial.println(maxY);

      maxY = receivedDetails.toFloat();

      Serial.print("New maxY: ");
      Serial.println(maxY);
    } else if(receivedCommand == "MINY") {
      Serial.print("Old minY: ");
      Serial.println(minY);

      minY = receivedDetails.toFloat();

      Serial.print("New minY: ");
      Serial.println(minY);
    }
  }

  // State 3: Do all possible functions since you've been deployed
  if(isConnected && isDeployed) {

    // State 1: Continuously moving
    if(!hasStopped && !hasDetectedObject) {

      if(millis() - startTime > 1000) {
        digitalWrite(greenLed, !digitalRead(greenLed));
        startTime = millis();
      } 

      // State 2: Maneuvering. If Y reaches limit, turn
      if(currentY > maxY) {
        savedAngle = oppositeStraightAngle;

        leftAngle = savedAngle + detectAngle;
        if(leftAngle > 360) leftAngle = leftAngle - 360;
        if(leftAngle < 0) leftAngle = 360 + leftAngle;

        rightAngle = savedAngle - detectAngle;
        if(rightAngle > 360) rightAngle = rightAngle - 360;
        if(rightAngle < 0) rightAngle = 360 + rightAngle;

        Serial.print("Saved angle: ");
        Serial.println(savedAngle);
      } else if(currentY < minY) {
        savedAngle = straightAngle;

        leftAngle = savedAngle + detectAngle;
        if(leftAngle > 360) leftAngle = leftAngle - 360;
        if(leftAngle < 0) leftAngle = 360 + leftAngle;

        rightAngle = savedAngle - detectAngle;
        if(rightAngle > 360) rightAngle = rightAngle - 360;
        if(rightAngle < 0) rightAngle = 360 + rightAngle;

        Serial.print("Saved angle: ");
        Serial.println(savedAngle);
      }

      tempAngle = savedAngle;
      move(currentAngle);
    }

    // State 2: Detected something, so move there
    if(!hasStopped && hasDetectedObject) {
      if(millis() - startTime2 > 5000) {
        hasStopped = false;
        hasDetectedObject = false;
        startTime = millis();
        startTime2 = millis();
        
        digitalWrite(greenLed, LOW);
        digitalWrite(blueLed, LOW);
        digitalWrite(yellowLed, LOW);
        digitalWrite(redLed, LOW);

        savedAngle = tempAngle;
      }
      move(currentAngle);
    }

    // State 3: Stop moving
    if(hasStopped && !isGoingHome) {
      if(startTime - millis() > 1000) {
        digitalWrite(greenLed, LOW);
        digitalWrite(yellowLed, LOW);
        digitalWrite(blueLed, LOW);
        digitalWrite(redLed, !digitalRead(redLed));

        startTime = millis();
      }
    }  

    // State 3: Is going home 
    if(hasStopped && isGoingHome) {
      if(abs(currentX - homeX) > 1.5) {
        // start moving to home x
        if(currentX - homeX < 0) {
          savedAngle = rightAngle;

          digitalWrite(greenLed, LOW);
          digitalWrite(yellowLed, LOW);
          digitalWrite(blueLed, HIGH);
          digitalWrite(redLed, LOW);

          Serial.println("Move right");
        } else if(currentX - homeX > 0) {
          savedAngle = leftAngle;

          digitalWrite(greenLed, LOW);
          digitalWrite(yellowLed, HIGH);
          digitalWrite(blueLed, LOW);
          digitalWrite(redLed, LOW);

          Serial.println("Move left");
        }
        move(currentAngle);
      } else if(abs(currentY - homeY) > 1.5) {
        // start moving to home y
        savedAngle = oppositeStraightAngle;

        digitalWrite(greenLed, LOW);
        digitalWrite(yellowLed, HIGH);
        digitalWrite(blueLed, HIGH);
        digitalWrite(redLed, LOW);

        Serial.println("Turn back");
        move(currentAngle);
      } else {
        isGoingHome = false;
        hasStopped = true;

        Serial.println("Reached home");

        escRight.write(stopSpeed);
        escLeft.write(stopSpeed);
      }
    }

  }

}

///////Specific functions/////////
void move(float currentAngle) {
  float error = abs(currentAngle - savedAngle);
  float previous_error;
  float cumulative_error;

  float PID_p = kp * error;
  float PID_i = cumulative_error * ki;
  float PID_d = kd*(error - previous_error);

  double PID_total = PID_p + PID_i + PID_d;

  cumulative_error += error;
  previous_error = error;
  
  float modifiedSpeed = PID_total;

  // Sets a limit for max and min speed
  if(modifiedSpeed >= maxSpeed) {
    modifiedSpeed = maxSpeed;
  } else if(modifiedSpeed <= minSpeed) {
    modifiedSpeed = minSpeed;
  }

  bool isStraight = false;
  bool isLeft = false;

  if(error < 10) isStraight = true;
  else isStraight = false;
  
  if(isStraight) {
    if(isLeft) {
      escLeft.write(modifiedSpeed+5);
      escRight.write(minSpeed);
    } else {
      escLeft.write(minSpeed);
      escRight.write(modifiedSpeed);
    }
  } else {
    float oppositeSavedAngle = savedAngle + 180;
    float oppositeCurrentAngle = 360-currentAngle;

    if(oppositeSavedAngle > 360) {
      oppositeSavedAngle = oppositeSavedAngle - 360;
    }

    isLeft =  !(((currentAngle > savedAngle) && (currentAngle <= oppositeSavedAngle)) || 
              ((oppositeSavedAngle < savedAngle) && (currentAngle > savedAngle) && (currentAngle <= 360)) ||
              ((oppositeSavedAngle < savedAngle) && (currentAngle < oppositeSavedAngle)))
              ;

    if(isLeft) {
      escLeft.write(modifiedSpeed+5);
      escRight.write(stopSpeed);
    } else {      
      escLeft.write(stopSpeed);
      escRight.write(modifiedSpeed);
    }
  }
}

///////General functions/////////
bool receiveCommand() {
  while(Nano.available()) {
    char letter = Nano.read();
    if(letter == '\n') {
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