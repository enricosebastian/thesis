#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <LinkedList.h>

SoftwareSerial HC12(6, 7); // HC-12 TX Pin, HC-12 RX Pin
LinkedList<String> drones;

const String myName = "BaseStation";
const int deployBtn = 13;
StaticJsonDocument<200> received; //Only received strings need to be global variables...

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);
  Serial.println("Base station initializing...");
}

void loop() {
  
}

void forDrone() {
  if(sentCommandSuccessfully("TEST", "BaseStation", "HELL")) {
    Serial.println("Sent and acknowledged!");
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
      Serial.println("===============================");
      return true;
    } else {
      Serial.println("====Received a choppy====");
      
      Serial.print("receivedCommand: ");
      Serial.println(received["command"].as<String>());
    
      Serial.print("receivedToName: ");
      Serial.println(received["toName"].as<String>());
    
      Serial.print("receivedFromName: ");
      Serial.println(received["fromName"].as<String>());

      Serial.print("receivedDetails: ");
      Serial.println(received["details"].as<String>());
      Serial.println("===============================");
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
  Serial.println("=====================");
  
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
    if((millis() - startTime) >= 5000) {
      Serial.println("sentCommandSuccessfully: Waited too long...");
      return false;
    }
  }
  return true;
}

bool receivedSpecificCommand(String command) {
  unsigned long startTime = millis(); //Take the time now. Save for later.
  while(!(receivedCommand() && received["toName"].as<String>() == myName && received["command"].as<String>() == command)) {
    if((millis() - startTime) >= 5000) {
      Serial.println("receivedSpecificCommand: Waited too long...");
      return false;
    }
  }
  Serial.println("Received intended command. Sending acknowledgement");
  startTime = millis();
  while((millis() - startTime) <= 5000) {
    sendCommand(received["command"].as<String>()+"REP", received["fromName"].as<String>(), "SUCC");
  }
  return true;
}
