#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <LinkedList.h>

SoftwareSerial HC12(6, 7); // HC-12 TX Pin, HC-12 RX Pin

StaticJsonDocument<200> sent;
StaticJsonDocument<200> received;

LinkedList<String> drones;

String command = "";
String message = "";
String owner = "";

bool readCommand() {
  DeserializationError err = deserializeJson(received, HC12);
  if(err == DeserializationError::Ok) {
    command = received["COMMAND"].as<String>();
    message = received["MESSAGE"].as<String>();
    owner = received["OWNER"].as<String>();
    return true;
  } else {
    Serial.print(err.c_str());
    while(HC12.available() > 0) {
      HC12.read();
    }
  }
  return false;
}

bool sendMessage(String cmd, String msg) {
  sent["COMMAND"] = cmd;
  sent["MESSAGE"] = msg;
  sent["OWNER"] = "basestation";
  serializeJson(sent, HC12);
  return true;
}

bool hasReceivedMessage() {
  if(HC12.available()) {
    if(readCommand()) {
      return true;
    }
  }
  return false;
}

void setup() {
  Serial.begin(9600);
  HC12.begin(9600); 
  
  pinMode(10, OUTPUT); // DRONE 1
  pinMode(11, OUTPUT); // DRONE 2
  pinMode(12, OUTPUT); // DRONE 3

  pinMode(8, INPUT);
  pinMode(9, INPUT);
}

void searchForDrones() {
  if(hasReceivedMessage()) {
    if(command == "CONNECT") {
      addDrone();
    }
  }
}

void addDrone() {
  for(int i = 0; i< drones.size(); i++) {
    if(drones.get(i) == owner) {
      Serial.println("This drone has already been listed.");

      //remove later0
      digitalWrite(11, HIGH);
      delay(1000);
      digitalWrite(11, LOW);
      return;
    }
  }
  drones.add(owner);
  Serial.println("Successfully added drone.");
}

void startDeployment() {
  digitalWrite(12, HIGH);

  Serial.println("\n\nDeploying...");
  for(int i = 0; i< drones.size(); i++) {
    sendMessage("DEPLOY",drones.get(i));
    Serial.println(drones.get(i));
  }
  
  while(true) {
    // do nothing
  }
}

void loop() {
  if(digitalRead(8) == HIGH) {
    // stop searching for stations, and start deployment
    startDeployment();
  }
  
  //keep searching for drones
  searchForDrones();
}
