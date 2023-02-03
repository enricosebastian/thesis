#include <Arduino.h>
#include <NeoSWSerial.h>
#include <LinkedList.h>
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
const String myName = "BASE";
// const String myName = "DRO1";
// const String myName = "DRO2";
// const String myName = "DRO3";

//Constants (buttons)
const int redLed = 13;
const int yellowLed = 12;
const int greenLed = 11;
const int detectionPin = 10;
const int escLeftPin = 6;
const int escRightPin = 5;
const int btn = 7;
const int txPin = A0; //green tx
const int receivedPin = A1; //blue received
const int waitingTime = 5000;

const float minSpeed = 7;
const float movingSpeed = 15;
const float maxSpeed = 20;
const float maxAngleChange = 5;
const float turnDegrees = 90;

//Booleans for logic
bool isConnected = false;
bool isDeploying = false;
bool isDeployed = false;
bool isAcknowledging = false;
bool isGoingHome = false;
bool hasReceivedCommand = false;
bool hasDetectedObject = false;
bool isLeft = false;

//Variables
unsigned long startTime = 0;

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

NeoSWSerial HC12(txPin, receivedPin); // (Green TX, Blue RX)
LinkedList<String> drones;
Servo escLeft;
Servo escRight;

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);
  Serial.print(myName);
  Serial.println(" is initializing...");
  
  //GPIO initialization
  pinMode(redLed, OUTPUT);
  digitalWrite(redLed, HIGH);
  
  pinMode(yellowLed, OUTPUT);
  digitalWrite(yellowLed, HIGH);
  
  pinMode(greenLed, OUTPUT);
  digitalWrite(greenLed, HIGH);

  pinMode(detectionPin, OUTPUT);
  digitalWrite(detectionPin, LOW);

  pinMode(btn, INPUT);

  delay(500);

  //Successful intialization indicator
  if(myName == "BASE") {
    //For base station, no LED should turn on
    digitalWrite(redLed, LOW);
    digitalWrite(yellowLed, LOW);
    digitalWrite(greenLed, LOW);
  } else {
    //For drone, it should be on a RED LED
    digitalWrite(redLed, HIGH);
    digitalWrite(yellowLed, LOW);
    digitalWrite(greenLed, LOW);

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
  }

  Serial.print(myName);
  Serial.println(" has initialized.");
  startTime = millis();
}

void loop() {
  if(myName == "BASE") {
    forBaseStation();
  } else {
    forDrone();
  }
}

void forBaseStation() {
  //TASK 1: If you are not yet deploying, capture all the drones that want to connect with you.
  if(!isDeploying && !isDeployed && !isAcknowledging) {
    if(receivedSpecificCommand("CONN")) {
      Serial.print(receivedFromName);
      Serial.println(" wanted to connect. Sending handshake.");
      addDrone(receivedFromName);
      isAcknowledging = true;
      startTime = millis();
    }
  }

  //TASK 1.5: If you received a new drone name, don't forget to acknowledge its presence with a handshake.
  if(!isDeploying && !isDeployed && isAcknowledging && (millis() - startTime <= waitingTime)) {
    if(millis() - startTime >= 800) {
      sendCommand("CONNREP", receivedFromName, "SUCC");
      startTime = millis();
    }
  } else if(!isDeploying && !isDeployed && isAcknowledging && (millis() - startTime > waitingTime)) {
    isAcknowledging = false;

    //Drone size indicator
    if(drones.size() == 1) {
      digitalWrite(redLed, HIGH);
      digitalWrite(yellowLed, LOW);
      digitalWrite(greenLed, LOW);
    } else if(drones.size() == 2) {
      digitalWrite(redLed, HIGH);
      digitalWrite(yellowLed, HIGH);
      digitalWrite(greenLed, LOW);
    } else if(drones.size() == 3) {
      digitalWrite(redLed, HIGH);
      digitalWrite(yellowLed, HIGH);
      digitalWrite(greenLed, HIGH);
    } else {
      digitalWrite(redLed, LOW);
      digitalWrite(yellowLed, LOW);
      digitalWrite(greenLed, LOW);
    }
  }

  //TASK 2: If you pressed the button, start deployment.
  if(!isDeploying && !isDeployed && !isAcknowledging && digitalRead(btn) == HIGH) {
    isDeploying = true;
    Serial.println("Button pressed. Starting deployment.");
    digitalWrite(redLed, LOW);
    digitalWrite(yellowLed, HIGH);
    digitalWrite(greenLed, LOW);
  }

  //TASK 3: Deployment starts by messaging all the drones to start moving.
  if(isDeploying && !isDeployed) {
    for(int i = 0; i < drones.size(); i++) {    
      //Reset values first 
      receivedFromName = "";
      receivedCommand = "";
      receivedToName = "";
      receivedDetails = "";

      Serial.print("Trying to deploy ");
      Serial.println(drones.get(i));
      Serial.println("\n");

      startTime = millis();
      sendCommand("DEPL", drones.get(i), "HELL");
      
      while(!receivedSpecificCommand("DEPLREP") && receivedFromName != drones.get(i)) {
        if(millis() - startTime >= waitingTime) {
          startTime = millis();
          Serial.print("Did not receive 'DEPLREP' from '");
          Serial.print(drones.get(i));
          Serial.println("' yet. Sending 'DEPL' again.");
          sendCommand("DEPL", drones.get(i), "HELL");
        }
      }
      Serial.print("Successfully deployed: ");
      Serial.println(drones.get(i));
    }

    digitalWrite(redLed, LOW);
    digitalWrite(yellowLed, LOW);
    digitalWrite(greenLed, HIGH);

    Serial.println("All drones have been deployed. You can start sending commands now.");
    isDeployed = true;
    startTime = millis();
    digitalWrite(detectionPin, HIGH);
  }

  //TASK 4: Since every drone is deployed, try to send them random commands while they're running.
  if(isDeploying && isDeployed) {
    if(Serial.available()) {
      String input = Serial.readStringUntil('\n');
      Serial.println(input);

      int endIndex = input.indexOf(' '); 

      String command = input.substring(0, endIndex);
      input = input.substring(endIndex+1);

      endIndex = input.indexOf(' ');
      String toName = input.substring(0, endIndex);
      String details = input.substring(endIndex+1);

      sendCommand(command, toName, details);
      startTime = millis();
      while(!receivedSpecificCommand(command+"REP")) {
        if(millis() - startTime > 800) {
          Serial.print(command);
          Serial.println("REP was not yet received. Resending commmand.");
          sendCommand(command, toName, details);
          startTime = millis();
        }

        if(Serial.available() && Serial.readStringUntil('\n') != "smnth") {
          Serial.println("Canceling sending of command. Try sending a new command.");
          break;
        }
      }
    }
    
    if(receiveCommand()) {       
    }
  }
}

void forDrone() {
  //TASK 1: Keep sending connect command until acknowledged.
  if(!isConnected && !isDeployed) {
    if(!receivedSpecificCommand("CONNREP")) {
      if(millis() - startTime >= waitingTime) {
        Serial.println("Reply 'CONNREP' was not received. Resending message again.");
        sendCommand("CONN", "BASE", "HELL");
        startTime = millis();
      }
    } else {
      Serial.println("Successfully detected by base station. Now we wait for deployment.");
      isConnected = true;
    }
  }

  //TASK 2: Wait for base station to send deploy command to start moving.
  if(isConnected && !isAcknowledging && !isDeployed) {
    if(!receivedSpecificCommand("DEPL")) {
      if(millis() - startTime >= waitingTime) {
        Serial.println("Command 'DEPL' was not received yet. Continue waiting.");
        startTime = millis();
      }
    } else {
      Serial.println("Base station wants to start deploying.");
      isAcknowledging = true;
      startTime = millis();
      initialAngle = Compass.GetHeadingDegrees(); //243
    }
    digitalWrite(redLed, LOW);
    digitalWrite(yellowLed, HIGH);
    digitalWrite(greenLed, LOW);
  }

  //TASK 2.1: Base station wants to deploy us. Send acknowledgement/handshake for at least 5 seconds
  if(isConnected && isAcknowledging && !isDeployed) {
    if(millis() - startTime <= waitingTime) {
      if(millis() - startTime >= 800) {
        sendCommand("DEPLREP", receivedFromName, "SUCC");
      }
    } else if(millis() - startTime > waitingTime) {
      escLeft.write(minSpeed);
      escRight.write(minSpeed);
      digitalWrite(detectionPin, HIGH);
      Serial.println("Drone is deploying. Moving motors.");
      isAcknowledging = false;
      isDeployed = true;
      startTime = millis();
    }
  }
  
  //TASK 3: Start moving. Plus, look for commands from base station. And also RPi.
  if(isConnected && !isAcknowledging && isDeployed) {

    //TASK 3.1: If you received a command from base station, stop what you are doing and interpret the command.
    if(!hasDetectedObject && !hasReceivedCommand && receiveCommand()) {
      hasReceivedCommand = true;
      startTime = millis();
    }

    //TASK 3.1.2: Read what each command means
    if(!hasDetectedObject && hasReceivedCommand) {
      if(millis() - startTime <= waitingTime) {
        // Send acknowledgement that you received a command
        sendCommand(receivedCommand+"REP", receivedFromName, "SUCC");
      } else if(millis() - startTime > waitingTime) {
        // Turn off hasReceivedCommand, and then interpret the actual command
        hasReceivedCommand = false;
        
        if(receivedCommand == "STOP") {
          Serial.println("Stopping drone.");
          digitalWrite(redLed, HIGH);
          digitalWrite(yellowLed, LOW);
          digitalWrite(greenLed, LOW);
          
          isGoingHome = true;
          escLeft.write(0);
          escRight.write(0);
        } else if(receivedCommand == "GO") {
          Serial.println("Drone resuming deployment.");
          digitalWrite(redLed, LOW);
          digitalWrite(yellowLed, LOW);
          digitalWrite(greenLed, HIGH);
          
          isGoingHome = false; // Revert status back to false
          initialAngle = Compass.GetHeadingDegrees(); // Save new angle
          escLeft.write(minSpeed); //re-initialize escs
          escRight.write(minSpeed);
        } else if(receivedCommand == "TURN") {
            initialAngle = initialAngle+receivedDetails.toInt(); // add value of details
            Serial.print("Saved angle changed. New angle is: ");
            Serial.println(initialAngle);
        }
      }
    }

    //TASK 3.2: Ensure that you are moving
    if(!hasDetectedObject && !hasReceivedCommand && !isGoingHome) {
      if(millis() - startTime >= 1000) {
        startTime = millis();
        digitalWrite(redLed, LOW);
        digitalWrite(yellowLed, LOW);
        digitalWrite(greenLed, !digitalRead(greenLed));
      }
      moveDrone(); 
    }
    
    //TASK 3.3: Look for RPi commands (Check if you detect an object in the water)
    if(!hasDetectedObject && !hasReceivedCommand && !isGoingHome && detectObject()) {
      hasDetectedObject = true;
      startTime = millis();
    }
    
    if(!hasReceivedCommand && hasDetectedObject && !isGoingHome) {
      //Basically deconstruct the details of the object and its location here
      if(receivedDetails == "LEFT") {
        initialAngle = initialAngle-20;
        moveDrone();
      } else if(receivedDetails == "CENTER") {
        initialAngle = initialAngle;
        moveDrone();
      } else if(receivedDetails == "RIGHT") {
        initialAngle = initialAngle+20;
        moveDrone();
      }
    }
    
    //Task 3.4: If command says to stop, then stop the prototype.
    if(!hasReceivedCommand && !hasDetectedObject && isGoingHome) {
      //Do nothing lmao. Go home
    }
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

void addDrone(String droneName) {
  //Check first if drone already exists in list
  for(int i = 0; i < drones.size(); i++) {
    if(drones.get(i) == droneName) {
      Serial.print(droneName);
      Serial.println(" is already connected!");
      return;
    }
  }
  drones.add(droneName); //Successfully added this drone.
}

///////General functions/////////
bool receiveCommand() {
  while(HC12.available()) {
    char letter = HC12.read();
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
    Serial.print("Sending: ");
    Serial.println(sentMessage);
    HC12.println(sentMessage);
  } else {
    Serial.println("Wrong format of command. Try again.");
  }
  
}

bool receivedSpecificCommand(String command) {
  return receiveCommand() && (receivedCommand == command);
}

// fix in next update
bool detectObject() {
  if(Serial.available()){
    receivedMessage = Serial.readStringUntil("\n");
    
    int endIndex = receivedMessage.indexOf(' ');
    receivedCommand = receivedMessage.substring(0, endIndex);
    receivedMessage = receivedMessage.substring(endIndex+1);
    
    endIndex = receivedMessage.indexOf(' ');
    receivedToName = receivedMessage.substring(0, endIndex);
    receivedMessage = receivedMessage.substring(endIndex+1);
    
    endIndex = receivedMessage.indexOf(' ');
    receivedFromName = receivedMessage.substring(0, endIndex);
    receivedDetails = receivedMessage.substring(endIndex+1);
    Serial.println("Detected an object!");
    return true;
  }
  return false;
}
