#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <LinkedList.h>

SoftwareSerial HC12(6, 7); // HC-12 TX Pin, HC-12 RX Pin

StaticJsonDocument<200> sent;
StaticJsonDocument<200> received;

LinkedList<String> drones;

String input;
const String senderName = "BaseStation";

String recipientName = "";

int hasDeployed = 13;

String receivedCommand = "";

// Received JSON string format
String command = "";
String sender = "";
String details = "";
bool response = false;
////////////////////////////

int index = 0;
int motorSpeedLeft;
int motorSpeedRight;

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);
  Serial.println("System initializing...");
  pinMode(hasDeployed, INPUT);
}

void loop() {
  
  //Task 1: Find drones
  while(digitalRead(hasDeployed) == LOW){
    findDrones();
  }
  
  //Task 2: Deploy drones
  deployDrones();

  //Task 3: Continuously wait for messages sent by drones
  while(true) {
    Serial.println("boop");
  }
}

void deployDrones() {
  for(int i = 0; i < drones.size(); i++) {
    Serial.print("Deploying ");
    Serial.println(drones.get(i));
    
    sent["command"] = "DEPLOY";
    sent["details"] = drones.get(i);
    sent["to"] = drones.get(i);
    sent["response"] = true;
    while(!sendCommand(sent)) {
      //Keep sending the command until it's a success
    }
  }
}

void findDrones() {
  if(HC12.available()) {
    DeserializationError err = deserializeJson(received, HC12);
    if(err == DeserializationError::Ok) {
      Serial.println("Received a request...");
      receivedCommand = received["command"].as<String>();
      sender = received["from"].as<String>();
      if(receivedCommand == "CONNECT") {
        addDrone(sender);
      }
    }
  }
}

void addDrone(String drone) {
  //Task 1: Check if drone already exists in list
  for(int i = 0; i < drones.size(); i++) {
    if(drones.get(i) == drone) {
      Serial.println("This drone has already been listed");
      sent["command"] = "REPLY";
      sent["details"] = "Drone already exists in list";
      sent["response"] = false;
      
      while(!sendCommand(sent)) {
        //Keep sending the command until it's a success
      }
      return;
    }
  }

  //Task 2: If not, add it to the list
  drones.add(drone);
  Serial.println("Succesfully added drone");

  sent["command"] = "REPLY";
  sent["details"] = "Added drone";
  sent["to"] = drone;
  sent["response"] = true;
  
  while(!sendCommand(sent)) {
    //Keep sending the command until it's a success
  }
}

bool sendCommand(StaticJsonDocument<200> sent) {
  sent["from"] = senderName;
  serializeJson(sent, HC12);
  
  unsigned long startTime = millis();
  while(!receiveReply()) {
    Serial.println("Waiting for handshake...");
    if((millis() - startTime) >= 5000) {
      Serial.println("Handshake failed...");
      return false; //Stop waiting. Handshake failed.
    }
  }
  Serial.println("Handshake succeeded...");
  return true;
}

bool receiveReply() {
  if(HC12.available()) {
    DeserializationError err = deserializeJson(received, HC12);
    if(err == DeserializationError::Ok) {
      Serial.println("Received a clean command...");
      receivedCommand = received["command"].as<String>();
      response = received["response"].as<bool>();
      recipientName = received["to"].as<String>();
      return (receivedCommand == "REPLY" && response && recipientName == senderName);
    } else {
      Serial.println("Received a choppy command...");
    }
  }
  return false;
}

//void loop() {
//  if(Serial.available()) {
//    input = Serial.readStringUntil('\n');
//    index = input.indexOf(' ');
//    
//    motorSpeedLeft = input.substring(0, index).toInt();
//    motorSpeedRight =input.substring(index+1).toInt();
//
//    if(motorSpeedLeft > 180) motorSpeedLeft = 180;
//    if(motorSpeedLeft < 0) motorSpeedLeft = 0;
//    if(motorSpeedRight > 180) motorSpeedRight = 180;
//    if(motorSpeedRight < 0) motorSpeedRight = 0;
//
//    Serial.print("{");
//    Serial.print(motorSpeedLeft);
//    Serial.print(", ");
//    Serial.print(motorSpeedRight);
//    Serial.println("}");
//    Serial.println("\n{Left motor, Right motor}:");
//    if(motorSpeedLeft == 0 && motorSpeedRight == 0) {
//      sent["command"] = "STOP";
//    } else {
//      sent["command"] = "GO";
//    }
//    sent["motorSpeedLeft"] = motorSpeedLeft;
//    sent["motorSpeedRight"] = motorSpeedRight;
//    sent["duration"] = 1;
//    sent["controllerDelay"] = 500;
//    serializeJson(sent, HC12);
//  }
//
//  if(HC12.available()) {
//    DeserializationError err = deserializeJson(received, HC12);
//    if(err == DeserializationError::Ok) {
//      int motorSpeedLeftReceived = received["motorSpeedLeft"].as<int>();
//      int motorSpeedRightReceived = received["motorSpeedRight"].as<int>();
//      float mainHeadingReceived = received["mainHeading"].as<float>();
//      float headingReceived = received["heading"].as<float>();
//      String commentReceived = received["comment"].as<String>();
//
//      Serial.print("Received:    ");
//      Serial.print(motorSpeedLeftReceived);
//      Serial.print(" - ");
//      Serial.print(motorSpeedRightReceived);
//      Serial.print(" - ");
//      Serial.print(commentReceived);
//      Serial.print(" - ");
//      Serial.print(mainHeadingReceived);
//      Serial.print(" - ");
//      Serial.println(headingReceived);
//    }
//  }
//}
