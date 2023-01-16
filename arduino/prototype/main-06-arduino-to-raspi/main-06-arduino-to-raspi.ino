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
 * SCL  -> A5/SCL, (Use Pin 21 on the Arduino Mega)
 * SDA  -> A4/SDA, (Use Pin 20 on the Arduino Mega)
*/

SoftwareSerial HC12(8, 9); // (Green TX, Blue RX)
LinkedList<String> drones;
Servo escLeft;
Servo escRight;
StaticJsonDocument<200> received; //Only received strings need to be global variables...

const String myName = "DRO1"; //Change name here

const int redLed = 13;
const int yellowLed = 12;
const int greenLed = 11;
const int detectionPin = 10;
const int escLeftPin = 7;
const int escRightPin = 6;
const int btn = 7;

bool isConnected = false;
bool isDeploying = false;
bool isDeployed = false;
bool isAcknowledging = false;
bool hasReceivedCommand = false;
bool hasDetectedObject = false;

unsigned long startTime = 0;

int posX = 0;
int posY = 0;
int escLeftSpeed = 0;
int escRightSpeed = 0;

float initialAngle = 0;

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
    Compass.SetDeclination(-2, 37, 'W');
    Compass.SetSamplingMode(COMPASS_SINGLE);
    Compass.SetScale(COMPASS_SCALE_130);
    Compass.SetOrientation(COMPASS_HORIZONTAL_X_NORTH);
    
    //ESC initialization
    escLeft.attach(escLeftPin,1000,2000);
    escRight.attach(escRightPin,1000,2000);
    escLeft.write(escLeftSpeed);
    escRight.write(escRightSpeed);
  }

  delay(500);
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
      while(!receivedSpecificCommand(command+"REP")) {
        if(millis() - startTime >= 5000) {
          startTime = millis();
          sendCommand(command, toName, details);
        }
      }
      Serial.println("Command sent successfully!");
    }

    if(receivedCommand()) {        
    }
  }
}

void forDrone() {
  //TASK 1: Keep sending connect command until acknowledged.
  if(!isConnected && !isDeployed) {
    if(!receivedSpecificCommand("CONNREP")) {
      if(millis() - startTime >= 5000) {
        Serial.println("Reply 'CONNREP' was not received. Resending message again.");
        startTime = millis();
        sendCommand("CONN", "BASE", "HELL");
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
      if(millis() - startTime >= 5000) {
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
    }
  }

  //TASK 3: Keep sending acknowledgements at least for 5 seconds or so...
  if(isConnected && isAcknowledging && !isDeployed) {
    if(millis() - startTime <= 10000) {
      sendCommand("DEPLREP", received["fromName"].as<String>(), "SUCC");
    } else if(millis() - startTime > 10000) {
      isAcknowledging = false;
      isDeployed = true;
      digitalWrite(detectionPin, HIGH);
      startTime = millis();
    }
  }

  //TASK 3: Start moving. All the while, look for commands.
  if(isConnected && !isAcknowledging && isDeployed) {

    //TASK 3.1: Continue reading for wireless commands coming from base station
    if(!hasDetectedObject && !hasReceivedCommand && receivedCommand()) {
      hasReceivedCommand = true;
      startTime = millis();
    }

    if(!hasDetectedObject && hasReceivedCommand) {
      if(millis() - startTime <= 10000) {
        sendCommand(received["command"].as<String>()+"REP", received["fromName"].as<String>(), "SUCC");
      } else if(millis() - startTime > 10000) {
        hasReceivedCommand = false;
      }
      
      if(received["command"].as<String>() == "ALL") {
        digitalWrite(redLed, HIGH);
        digitalWrite(yellowLed, HIGH);
        digitalWrite(greenLed, HIGH);
      }
    }

    //TASK 3.2: Ensure that you are moving
    if(!hasDetectedObject && !hasReceivedCommand) {
      String coordMessage = "(" + String(posX) + "," + String(posY) + ")";
      Serial.print("Direction: ");
      Serial.println(Compass.GetHeadingDegrees());
      if(millis() - startTime >= 1000) {
        startTime = millis();
        digitalWrite(redLed, !digitalRead(redLed));
        digitalWrite(yellowLed, LOW);
        digitalWrite(greenLed, LOW);
  
        if(posY % 2 == 0) {
          if(posX < 10)
            posX++;
          else 
            posY++;
        } else if(posY % 2 != 0) {
          if(posX > 0)
            posX--;
          else
           posY++;
        }
      }
    }

    //TASK 3.3: Watch out for any objects found by Raspi
    if(!hasDetectedObject && !hasReceivedCommand && detectedObject()) {
      hasDetectedObject = true;
      startTime = millis();
    }

    if(!hasReceivedCommand && hasDetectedObject) {
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
