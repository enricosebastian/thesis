#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <LinkedList.h>

SoftwareSerial HC12(6, 7); // HC-12 TX Pin, HC-12 RX Pin
LinkedList<String> drones;

const String myName = "DRO1";
const int waitingTime = 20000; //in milliseconds
StaticJsonDocument<200> received; //Only received strings need to be global variables...

const int redLed = 13;
const int greenLed = 12;

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);
  Serial.println("Base station initializing...");

  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  digitalWrite(greenLed, LOW);
  digitalWrite(redLed, HIGH);
  
  //for drone
  
}

void loop() {
  //for base station
  
}

void forDrone() {
  if(sentCommandSuccessfully("TEST", "BaseStation", "HELL")) {
    Serial.println("Sent and acknowledged!");
    digitalWrite(greenLed, HIGH);
    digitalWrite(redLed, LOW);
    while(true) {
      //do nothing
    }
  }
}

void forBaseStation() {
  while(!receivedSpecificCommand("TEST")) {
    //continue waiting
  }
  Serial.println("Received command we wanted!");
  digitalWrite(greenLed, HIGH);
  digitalWrite(redLed, LOW);
  while(true) {
    //do nothing now
  }
}

///////General functions/////////
bool receivedCommand() {
  if(HC12.available()) {
    DeserializationError err = deserializeJson(received, HC12); //Deserialize it into different possible variables
    
    if(err == DeserializationError::Ok) {
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
      return true;
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
  while(!(receivedCommand() && received["toName"].as<String>() == myName && received["command"].as<String>() == command+"REP")) {
    sendCommand(command, toName, details);
    if((millis() - startTime) >= waitingTime) {
      Serial.println("sentCommandSuccessfully: Waited too long...\n");
      return false;
    }
  }
  return true;
}

bool receivedSpecificCommand(String command) {
  unsigned long startTime = millis(); //Take the time now. Save for later.
  while(!(receivedCommand() && received["toName"].as<String>() == myName && received["command"].as<String>() == command)) {
    if((millis() - startTime) >= waitingTime) {
      Serial.println("receivedSpecificCommand: Waited too long...");
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
