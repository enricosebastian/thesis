#include <Arduino.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <LinkedList.h>
#include <Servo.h>
#include <Wire.h>
#include <HMC5883L_Simple.h>

HMC5883L_Simple Compass;
/*
 * GY-273 Compass Module  ->  Arduino
 * VCC  -> VCC  (See Note Below)
 * GND  -> GND
 * SCL  -> A5, blue
 * SDA  -> A4, green
*/

// Name here
//const String myName = "BASE";
const String myName = "DRO1";

const int redLed = 13;
const int yellowLed = 12;
const int greenLed = 11;
const int detectionPin = 10;
const int escLeftPin = 6;
const int escRightPin = 5;
const int btn = 7;
const int txPin = A0; //green tx
const int rxPin = A1; //blue rx
const int waitingTime = 5000;

const float minSpeed = 11;
const float maxSpeed = 90;
const float maxAngleChange = 5;

bool isConnected = false;
bool isDeploying = false;
bool isDeployed = false;
bool isAcknowledging = false;
bool isGoingHome = false;
bool hasReceivedCommand = false;
bool hasDetectedObject = false;

unsigned long startTime = 0;

int posX = 0;
int posY = 0;

float initialAngle = 0;
float kp = 8;
float ki = 0.2;
float kd = 10;
float PID_p, PID_i, PID_d, PID_total;

SoftwareSerial HC12(txPin, rxPin); // (Green TX, Blue RX)
LinkedList<String> drones;
Servo escLeft;
Servo escRight;
StaticJsonDocument<200> received; //Only received strings need to be global variables...

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);
  Serial.print(myName);
  Serial.println(" is initializing");
  
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

  //Successful intialization indicator
  if(myName == "BASE") {
    digitalWrite(redLed, LOW);
    digitalWrite(yellowLed, LOW);
    digitalWrite(greenLed, LOW);
  } else {
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
  }

  escLeft.write(0);
  escRight.write(0);
  startTime = millis();
}

void loop() {
//  forBaseStation();
  forDrone();
  // Serial.println(Compass.GetHeadingDegrees());
}

void forBaseStation() {
  //TASK 1: If you are not yet deploying, capture all the drones that want to connect with you.
  if(!isDeploying && !isDeployed && !isAcknowledging) {
    if(receivedSpecificCommand("CONN")) {
      Serial.println("Drone wanted to connect. Sending handshake.");
      addDrone(received["fromName"].as<String>());
      isAcknowledging = true;
      startTime = millis();
    }
  }

  //TASK 1.5: If you received a new drone name, don't forget to acknowledge its presence with a handshake.
  if(!isDeploying && !isDeployed && isAcknowledging && (millis() - startTime <= 5000)) {
    sendCommand("CONNREP", received["fromName"].as<String>(), "SUCC");
  } else if(!isDeploying && !isDeployed && isAcknowledging && (millis() - startTime > 5000)) {
    isAcknowledging = false;
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
      Serial.print("Trying to deploy ");
      Serial.println(drones.get(i));
      Serial.println("\n");

      startTime = millis();
      sendCommand("DEPL", drones.get(i), "HELL");
      
      while(!receivedSpecificCommand("DEPLREP") && !(received["fromName"].as<String>() == drones.get(i))) {
        if(millis() - startTime >= 5000) {
          startTime = millis();
          Serial.print("Deployment command was not acknowledged by '");
          Serial.print(drones.get(i));
          Serial.println("'. Trying again.");
          sendCommand("DEPL", drones.get(i), "HELL");
        }
      }
      Serial.print("Successfully deployed ");
      Serial.println(drones.get(i));
      Serial.println("\n");
    }
    Serial.println("All drones have been deployed. Start sending random commands.");
    digitalWrite(redLed, LOW);
    digitalWrite(yellowLed, LOW);
    digitalWrite(greenLed, HIGH);
    isDeployed = true;
    startTime = millis();
    digitalWrite(detectionPin, HIGH);
  }

  //TASK 4: Since every drone is deployed, try to send them random commands while they're running.
  if(isDeploying && isDeployed) {
    if(Serial.available()) {
      String input = Serial.readStringUntil('\n');
      int endIndex = input.indexOf(' '); 

      String command = input.substring(0, endIndex);
      input = input.substring(endIndex+1);

      endIndex = input.indexOf(' ');
      String toName = input.substring(0, endIndex);
      String details = input.substring(endIndex+1);
      startTime = millis();
      Serial.print("Sending: {");
      Serial.print(command);
      Serial.print(", ");
      Serial.print(toName);
      Serial.print(", ");
      Serial.print(details);
      Serial.println("}");
      
      while(!receivedSpecificCommand(command+"REP")) {
        if(millis() - startTime >= 5000) {
          Serial.println("No acknowledgement received. Resending again.");
          startTime = millis();
          sendCommand(command, toName, details);
        }
      }
      Serial.println("Command sent successfully!");
    }

    if(receivedCommand()) {       
      Serial.println("=========received======");
      Serial.print("command: ");
      Serial.println(received["command"].as<String>());
      Serial.print("toName: ");
      Serial.println(received["toName"].as<String>());
      Serial.print("fromName: ");
      Serial.println(received["fromName"].as<String>());
      Serial.print("details: ");
      Serial.println(received["details"].as<String>());
      Serial.println("===================");
    }
  }
}

void forDrone() {
  //TASK 1: Keep sending connect command until acknowledged.
  if(!isConnected && !isDeployed) {
    if(!receivedSpecificCommand("CONNREP")) {
      if(millis() - startTime >= waitingTime) {
        sendCommand("CONN", "BASE", "HELL");
        Serial.println("Reply 'CONNREP' was not received. Resending message again.");
        startTime = millis();
      }
    } else {
      isConnected = true;
      Serial.println("Successfully detected by base station. Waiting for deployment.");
      digitalWrite(redLed, LOW);
      digitalWrite(yellowLed, HIGH);
      digitalWrite(greenLed, LOW);
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
      isAcknowledging = true;
      Serial.println("Base station wants to start deploying.");
      startTime = millis();
      digitalWrite(redLed, LOW);
      digitalWrite(yellowLed, LOW);
      digitalWrite(greenLed, HIGH);
      initialAngle = Compass.GetHeadingDegrees(); //243
    }
  }

  //TASK 2.1: Base station wants to deploy us. Send acknowledgement/handshake for at least 5 seconds
  if(isConnected && isAcknowledging && !isDeployed) {
    if(millis() - startTime <= 10000) {
      sendCommand("DEPLREP", received["fromName"].as<String>(), "SUCC");
    } else if(millis() - startTime > 10000) {
      Serial.println("Drone is deploying. Moving motors.");

      escLeft.write(minSpeed);
      escRight.write(minSpeed);
      
      isAcknowledging = false;
      isDeployed = true;
      
      digitalWrite(detectionPin, HIGH);
      startTime = millis();
    }
  }
  
  //TASK 3: Start moving. Plus, look for commands from base station. And also RPi.
  if(isConnected && !isAcknowledging && isDeployed) {

    //TASK 3.1: If you received a command from base station, stop what you are doing and interpret the command.
    if(!hasDetectedObject && !hasReceivedCommand && receivedCommand()) {
      Serial.println("received a command");
      hasReceivedCommand = true;
      startTime = millis();
    }

    //TASK 3.1.2: Read what each command means
    if(!hasDetectedObject && hasReceivedCommand) {
      if(millis() - startTime <= 10000) {
        // send acknowledgement that you received a command
        sendCommand(received["command"].as<String>()+"REP", received["fromName"].as<String>(), "SUCC");
      } else if(millis() - startTime > 10000) {
        //turn off hasReceivedCommand and interpret the actual command
        hasReceivedCommand = false;
        
        if(received["command"].as<String>() == "STOP") {
          Serial.println("Stopping drone.");
          digitalWrite(redLed, LOW);
          digitalWrite(yellowLed, HIGH);
          digitalWrite(greenLed, LOW);
          
          isGoingHome = true;
          escLeft.write(0);
          escRight.write(0);
        } else if(received["command"].as<String>() == "GO") {
          Serial.println("Drone resuming deployment.");
          digitalWrite(redLed, LOW);
          digitalWrite(yellowLed, LOW);
          digitalWrite(greenLed, HIGH);
          
          isGoingHome = false; // Revert status back to false
          initialAngle = Compass.GetHeadingDegrees(); // Save new angle
          escLeft.write(minSpeed); //re-initialize escs
          escRight.write(minSpeed);
        } else if(received["command"].as<String>() == "TURN") {
          if(received["details"].as<String>() == "LEFT") {
            initialAngle = initialAngle-30; // add 90-degrees to the left
            Serial.println("Turning left.");
          } else if(received["details"].as<String>() == "RIGHT") {
            initialAngle = initialAngle+30; // add 90-degrees to the right
            Serial.println("Turning right.");
          }
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
      float modifiedSpeed = map(abs(PID_total),0,1600,minSpeed,maxSpeed);

     Serial.println(error);
      
      if(error < -maxAngleChange) {
        //It's turning right, so give the right motor more speed
        Serial.println("right");
        escLeft.write(minSpeed);
        escRight.write(modifiedSpeed);

//        Serial.println("=========");
//        Serial.print("LFTM: ");
//        Serial.println(minSpeed);
//
//        Serial.print("RITM: ");
//        Serial.println(modifiedSpeed);
//
//        Serial.print("INITA: ");
//        Serial.println(initialAngle);
//
//        Serial.print("CURRA: ");
//        Serial.println(Compass.GetHeadingDegrees());
//        Serial.println("=========");
        
//        sendCommand("LFTM", "BASE", String(minSpeed));
//        sendCommand("RITM", "BASE", String(modifiedSpeed));
//        sendCommand("INITA", "BASE", String(initialAngle));
//        sendCommand("CURRA", "BASE", String(Compass.GetHeadingDegrees()));
        
      } else if(error > maxAngleChange) {
        //It's turning left, so give the left motor more speed
         Serial.println("left");
        escLeft.write(modifiedSpeed);
        escRight.write(minSpeed);

//        Serial.println("=========");
//        Serial.print("LFTM: ");
//        Serial.println(modifiedSpeed);
//        
//        Serial.print("RITM: ");
//        Serial.println(minSpeed);
//
//        Serial.print("INITA: ");
//        Serial.println(initialAngle);
//
//        Serial.print("CURRA: ");
//        Serial.println(Compass.GetHeadingDegrees());
//        Serial.println("=========");

//        sendCommand("LFTM", "BASE", String(modifiedSpeed));
//        sendCommand("RITM", "BASE", String(minSpeed));
//        sendCommand("INITA", "BASE", String(initialAngle));
//        sendCommand("CURRA", "BASE", String(Compass.GetHeadingDegrees()));
      }
    }
    
    //TASK 3.3: Look for RPi commands (Check if you detect an object in the water)
    if(!hasDetectedObject && !hasReceivedCommand && !isGoingHome && detectedObject()) {
      hasDetectedObject = true;
      startTime = millis();
    }
    
    if(!hasReceivedCommand && hasDetectedObject && !isGoingHome) {
      if(millis() - startTime <= 10000) {
        //Do we need to acknowledge though? It's physically connected, so data is strong
        //sendCommand("DETEREP", received["fromName"].as<String>(), "SUCC");
      } else if(millis() - startTime > 10000) {
        hasDetectedObject = false;
        startTime = millis();
      }

      //Basically deconstruct the details of the object and its location here
      if(received["details"].as<String>() == "RED") {
        digitalWrite(redLed, HIGH);
        digitalWrite(yellowLed, LOW);
        digitalWrite(greenLed, LOW);
      } else if(received["details"].as<String>() == "YELL") {
        digitalWrite(redLed, LOW);
        digitalWrite(yellowLed, HIGH);
        digitalWrite(greenLed, LOW);
      } else if(received["details"].as<String>() == "GREE") {
        digitalWrite(redLed, LOW);
        digitalWrite(yellowLed, LOW);
        digitalWrite(greenLed, HIGH);
      }
    }
    
    //Task 3.4: If command says to stop, then stop the prototype.
    if(!hasReceivedCommand && !hasDetectedObject && isGoingHome) {
      //Do nothing lmao. Go home
    }
  }
}

///////Specific functions/////////
void addDrone(String droneName) {
  //Check first if drone already exists in list
  for(int i = 0; i < drones.size(); i++) {
    if(drones.get(i) == droneName)
      return;
  }
  drones.add(droneName); //Successfully added this drone.

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

///////General functions/////////
bool receivedCommand() {
  if(HC12.available()) {
    DeserializationError err = deserializeJson(received, HC12); //Deserialize it into different possible variables
    
    if(err == DeserializationError::Ok)
      return (received["toName"].as<String>() == myName);
    else
      return false;
  }

  //This means you received no command at all...
  received["command"] = "";
  received["toName"] = "";
  received["fromName"] = "";
  received["details"] = "";
  return false;
}

void sendCommand(String command, String toName, String details) {
  StaticJsonDocument<200> sent;
  sent["command"] = command;
  sent["toName"] = toName;
  sent["fromName"] = myName;
  sent["details"] = details;
  serializeJson(sent, HC12);
}

bool receivedSpecificCommand(String command) {
  return receivedCommand() && received["command"].as<String>() == command;
}

bool detectedObject() {
  if(Serial.available()){
    DeserializationError err = deserializeJson(received, Serial); //Deserialize it into different possible variables
    
    if (err == DeserializationError::Ok) 
      return (received["toName"].as<String>() == myName) && (received["command"].as<String>() == "DETE");
    else
      return false;
  }

  //This means you received no command at all...
  received["command"] = "";
  received["toName"] = "";
  received["fromName"] = "";
  received["details"] = "";
  return false;
}
