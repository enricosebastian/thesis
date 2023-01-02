#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <LinkedList.h>

SoftwareSerial HC12(6, 7); // HC-12 TX Pin, HC-12 RX Pin
LinkedList<String> drones;

const String myName = "BaseStation";
const int deployBtn = 13;

///////General functions/////////
StaticJsonDocument<200> receiveCommand() {
  StaticJsonDocument<200> received; //Create a JSON variable
  
  if(HC12.available()) {
    DeserializationError err = deserializeJson(received, HC12); //Deserialize it into different possible variables
    
    if(err == DeserializationError::Ok) {
      Serial.println("====Received a clean command====");
      
      Serial.print("receivedCommand: ");
      Serial.println(received["command"].as<String>());
    
      Serial.print("receivedToName: ");
      Serial.println(received["toName"].as<String>());
    
      Serial.print("receivedFromName: ");
      Serial.println(received["fromName"].as<String>());

      Serial.print("receivedDetails: ");
      Serial.println(received["details"].as<String>());
      Serial.println("========");
      return received;
    } else {
      Serial.println("Received a choppy command...");
      received["command"] = "";
      received["details"] = "CHOPP";
      return received;
    }
  }
  
  received["command"] = "";
  received["toName"] = "";
  received["fromName"] = "";
  received["details"] = "";
  return received;
}

void sendCommand(StaticJsonDocument<200> sent) {
  sent["fromName"] = myName; //Indicate that you're the sender

  Serial.println("====Sending a command====");
      
  Serial.print("sentCommand: ");
  Serial.println(sent["command"].as<String>());

  Serial.print("sentToName: ");
  Serial.println(sent["toName"].as<String>());

  Serial.print("sentFromName: ");
  Serial.println(sent["fromName"].as<String>());

  Serial.print("sentDetails: ");
  Serial.println(sent["details"].as<String>());
  Serial.println("========");
  serializeJson(sent, HC12);
}

bool acknowledgeCommand(StaticJsonDocument<200> sent, String acknowledgeCommand) {
  StaticJsonDocument<200> received; //Create a JSON variable
  unsigned long startTime = millis(); //Take the time now. Save for later.
  
  while( !(received["toName"].as<String>() == myName && received["command"].as<String>() == acknowledgeCommand) ) {
    received = receiveCommand();
    sendCommand(sent);
    if(millis() - startTime == 5000) {
      Serial.println("Acknowledgement timed out.");
      return false;
    }
  }
  return true;
}

StaticJsonDocument<200> waitForCommand(String command) {
  StaticJsonDocument<200> received; //Create a JSON variable
  unsigned long startTime = millis(); //Take the time now. Save for later.
  
  while( !(received["toName"].as<String>() == myName && received["command"].as<String>() == command) ) {
    received = receiveCommand();
    if(millis() - startTime == 5000) {
      Serial.println("Waiting timed out.");
      received["command"] = "";
      received["toName"] = "";
      received["fromName"] = "";
      received["details"] = "";
      return received;
    }
  }

  Serial.println("Command captured. Sending final reply");
  StaticJsonDocument<200> sent; //Create a JSON variable
  sent["command"] = received["command"].as<String>()+"REP";
  sent["toName"] = received["fromName"].as<String>();
  sent["details"] = "NICE";
  for(int i=0; i<5; i++) {
    sendCommand(sent);
  }
  
  return received;
}

///////Drone-specific functions/////////
void lookForDrones() {
  String waitCommand = "DEPL";
  StaticJsonDocument<200> received = waitForCommand(waitCommand);
  if(received["toName"].as<String>() == myName && received["command"].as<String>() == waitCommand) {
    //add to list
    addDrone(received["fromName"].as<String>());
  }
}

void addDrone(String droneName) {
  //Check if drone already exists in List
  for(int i = 0; i < drones.size(); i++) {
    if(drones.get(i) == droneName) {
      Serial.print(droneName);
      Serial.println(" already exists.");
      return;
    }
  }

  //If not, add it to List
  drones.add(droneName);
  Serial.print(droneName);
  Serial.println(" successfully added.");
  return;
}

///////Main program/////////
void setup() {
  Serial.begin(9600);
  HC12.begin(9600);
  Serial.println("Drone initializing...");
}

void loop() {
  String waitCommand = "TEST";
  StaticJsonDocument<200> received = waitForCommand(waitCommand);
  if(received["toName"].as<String>() == myName && received["command"].as<String>() == waitCommand) {
    Serial.println("Received command.");
  }
}
