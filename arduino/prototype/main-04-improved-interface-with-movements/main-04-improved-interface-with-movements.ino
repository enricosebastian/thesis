#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <LinkedList.h>

SoftwareSerial HC12(8, 9); // (Green TX, Blue RX)
LinkedList<String> drones;

const String myName = "DRO1"; //Change name here
const int waitingTime = 10000; //in milliseconds
StaticJsonDocument<200> received; //Only received strings need to be global variables...

const int redLed = 13;
const int yellowLed = 12;
const int greenLed = 11;
const int btn = 7;

bool isConnected = false;
bool isDeploying = false;
bool isDeployed = false;
bool isAcknowledging = false;

unsigned long startTime = 0;
String command = "";

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);
  Serial.print(myName);
  Serial.println(" is initializing");

  pinMode(redLed, OUTPUT);
  digitalWrite(redLed, HIGH);
  
  pinMode(yellowLed, OUTPUT);
  digitalWrite(yellowLed, HIGH);
  
  pinMode(greenLed, OUTPUT);
  digitalWrite(greenLed, HIGH);

  pinMode(btn, INPUT);

  delay(500);
  digitalWrite(redLed, HIGH);
  digitalWrite(yellowLed, LOW);
  digitalWrite(greenLed, LOW);
  startTime = millis();
}

void loop() {
  //for base station
  
  //for drone
  
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
    Serial.println("All drones have been deployed.");
    digitalWrite(redLed, LOW);
    digitalWrite(yellowLed, LOW);
    digitalWrite(greenLed, HIGH);
    isDeployed = true;
  }

  //TASK 4: Since every drone is deployed, try to send them random commands while they're running.
  if(isDeploying && isDeployed) {
    //do nothing
    if(Serial.available()) {
      String input = Serial.readStringUntil('\n');
      int endIndex = input.indexOf(' '); 

      String command = input.substring(0, endIndex);
      input = input.substring(endIndex+1);

      endIndex = input.indexOf(' ');
      String toName = input.substring(0, endIndex);
      String details = input.substring(endIndex+1);

      Serial.print("COMMAND: ");
      Serial.println(command);
      Serial.print("TO NAME: ");
      Serial.println(toName);
      Serial.print("DETAILS: ");
      Serial.println(details);
    }
  }
}

void forDrone() {
  //TASK 1: Keep sending connect command until acknowledged.
  if(!isConnected && !isDeployed) {
    if(!receivedSpecificCommand("CONNREP")) {
      if(millis() - startTime >= 5000) {
        Serial.println("Reply 'CONNEREP' was not received. Resending message again.");
        startTime = millis();
        sendCommand("CONN", "BaseStation", "HELL");
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
    }
  }

  //TASK 3: Keep sending acknowledgements at least for 5 seconds or so...
  if(isConnected && isAcknowledging && !isDeployed) {
    if(millis() - startTime <= 5000) {
      sendCommand("DEPLREP", received["fromName"].as<String>(), "SUCC");
    } else if(millis() - startTime > 5000) {
      isAcknowledging = false;
      isDeployed = true;
      digitalWrite(redLed, LOW);
      digitalWrite(yellowLed, LOW);
      digitalWrite(greenLed, HIGH);
    }
  }

  //TASK 3: Start moving. All the while, look for commands.
  if(isConnected && !isAcknowledging && isDeployed) {
    //continue reading for commands
    receivedCommand();
  }
}

///////Specific functions/////////
void addDrone(String droneName) {
  //Check first if drone already exists in list
  for(int i = 0; i < drones.size(); i++) {
    if(drones.get(i) == droneName) {
      Serial.print("\n");
      Serial.print(droneName);
      Serial.println(" was already added\n");
      return;
    }
  }
  drones.add(droneName);
  Serial.print("\nSuccessfully added ");
  Serial.print(droneName);
  Serial.println(" to the list\n\nCurrent drones:");
  for(int i = 0; i < drones.size(); i++) {
    Serial.println(drones.get(i));
  }
  Serial.println("===============\n");

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

void deployDrones() {
  for(int i = 0; i < drones.size(); i++) {
    Serial.print("Trying to deploy ");
    Serial.println(drones.get(i));
    Serial.println("\n");

    while(!sentCommandSuccessfully("DEPL", drones.get(i), "HELL")) {
      //do nothing
    }
    Serial.print("Successfully deployed ");
    Serial.println(drones.get(i));
    Serial.println("\n");
  }
}

///////General functions/////////
bool receivedCommand() {
  if(HC12.available()) {
    DeserializationError err = deserializeJson(received, HC12); //Deserialize it into different possible variables
    
    if(err == DeserializationError::Ok) {
      if(received["toName"].as<String>() == myName) {
        Serial.println("====Received a command====");
      
        Serial.print("receivedCommand: ");
        Serial.println(received["command"].as<String>());
      
        Serial.print("receivedToName: ");
        Serial.println(received["toName"].as<String>());
      
        Serial.print("receivedFromName: ");
        Serial.println(received["fromName"].as<String>());
  
        Serial.print("receivedDetails: ");
        Serial.println(received["details"].as<String>());
        Serial.println("===============================\n\n");
      } else {
        Serial.println("Received a command not for this entity");
      }
      return (received["toName"].as<String>() == myName);
    } else {
      Serial.println("====Received a choppy command====");
      
      Serial.print("receivedCommand: ");
      Serial.println(received["command"].as<String>());
    
      Serial.print("receivedToName: ");
      Serial.println(received["toName"].as<String>());
    
      Serial.print("receivedFromName: ");
      Serial.println(received["fromName"].as<String>());

      Serial.print("receivedDetails: ");
      Serial.println(received["details"].as<String>());
      Serial.println("===============================\n\n");
      return false;
    }
  }
  Serial.println("\nReceived no command...\n");
  received["command"] = "";
  received["toName"] = "";
  received["fromName"] = "";
  received["details"] = "";
  return false;
}

void sendCommand(String command, String toName, String details) {
  StaticJsonDocument<200> sent;

  Serial.println("====Sending a command====");
      
  Serial.print("command: ");
  Serial.println(command);

  Serial.print("toName: ");
  Serial.println(toName);

  Serial.print("fromName: ");
  Serial.println(myName);

  Serial.print("details: ");
  Serial.println(details);
  Serial.println("=====================\n\n");
  
  sent["command"] = command;
  sent["toName"] = toName;
  sent["fromName"] = myName;
  sent["details"] = details;
  serializeJson(sent, HC12);
}

bool sentCommandSuccessfully(String command, String toName, String details) {
  unsigned long startTime = millis();
  unsigned long lappedTime = millis();
  sendCommand(command, toName, details); //Initial sending
  while(!(receivedCommand() && received["command"].as<String>() == command+"REP")) {
    
    if(millis() - lappedTime >= 5000) {
      lappedTime = millis();
      Serial.println("No acknowledgement. Resending command\n");
      sendCommand(command, toName, details);
    }
    
    if((millis() - startTime) >= waitingTime) {
      Serial.println("sentCommandSuccessfully: Waited too long...\n");
      return false;
    }
  }
  return true;
}

bool receivedCommandSuccessfully(String command) {
  unsigned long startTime = millis(); //Take the time now. Save for later.
  while(!(receivedCommand() && received["command"].as<String>() == command)) {
    if((millis() - startTime) >= waitingTime) {
      Serial.println("receivedCommandSuccessfully: Waited too long...");
      return false;
    }
  }
  Serial.println("Received intended command. Sending acknowledgement");
  startTime = millis();
  while((millis() - startTime) <= waitingTime) {
    sendCommand(received["command"].as<String>()+"REP", received["fromName"].as<String>(), "SUCC");
  }
  return true;
}

bool receivedSpecificCommand(String command) {
  return receivedCommand() && received["command"].as<String>() == command;
}
