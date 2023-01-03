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
const int btn = 8;

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
  
  
}

void loop() {
  //for base station
  
  //for drone
  
}

void forBaseStation() {
  bool isDeployed = false;

  while(!isDeployed) {
    lookForDrones();
    isDeployed = (digitalRead(btn) == HIGH);
  }

  Serial.println("Button pressed. Starting deployment initialization.");

  if(isDeployed) {
    deployDrones();
    Serial.println("Deployed!");
    digitalWrite(greenLed, HIGH);
    digitalWrite(redLed, LOW);
    
    while(true) {
      //do nothing...
    }
  }  
}

void forDrone() {
  while(!sentCommandSuccessfully("CONN", "BaseStation", "HELL")) {
    //do nothing
  }
  Serial.println("\nSuccessfully detected by base station. Waiting for deployment.\n");
  digitalWrite(redLed, LOW);
  digitalWrite(yellowLed, HIGH);
  digitalWrite(greenLed, LOW);
  
  

  while(!receivedCommandSuccessfully("DEPL")) {
    //continue to wait for DEPL command
  }

  Serial.println("Deploying drone");
  digitalWrite(redLed, LOW);
  digitalWrite(yellowLed, LOW);
  digitalWrite(greenLed, HIGH);

  Serial.println("Moving. Also waiting for next command.");
  while (true) {
    //do nothing...
    
  }
}

///////Specific functions/////////
void lookForDrones() {
  if(receivedCommandSuccessfully("CONN")) {
    addDrone(received["fromName"].as<String>());
  }
}

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
