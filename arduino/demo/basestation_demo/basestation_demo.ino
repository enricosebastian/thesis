#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <LinkedList.h>

SoftwareSerial HC12(6, 7); // HC-12 TX Pin, HC-12 RX Pin

StaticJsonDocument<200> sent;
StaticJsonDocument<200> received;

LinkedList<String> drones;

String input;
const String myName = "BaseStation";

String recipientName = "";

int deployBtn = 13;
bool hasDeployed = false;

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
  pinMode(deployBtn, INPUT);
}

void loop() {
  //Task 1: Find drones
  while(!hasDeployed){
    findDrones();
    if(digitalRead(deployBtn) == HIGH) {
      hasDeployed = true;
    }
  }

  //Task 2: Deploy drones
  if(hasDeployed) {
    Serial.println("Deploying drones.");
    //deployDrones();
  }

  //Task 3: Continuously wait for messages sent by drones
  while(true) {
    Serial.println("Deployed!");
  }
}

void deployDrones() {
  for(int i = 0; i < drones.size(); i++) {
    Serial.print("Deploying ");
    Serial.println(drones.get(i));

    String sentCommand = "DEPLOY";
    String sentToName = drones.get(i);
    String sentDetails = "Initialize deployment.";
    bool sentRespone = true;
    
    while(!sendCommand(sentCommand, sentToName, sentDetails, sentResponse)) {
      //Keep sending the command until it's a success
    }
  }
}

void findDrones() {
  Serial.println("Finding drones...");
  if(receiveCommand()) {
    Serial.println("Found a drone.");
  }
}

void addDrone(String droneName) {
  //Task 1: Check if drone already exists in list
  for(int i = 0; i < drones.size(); i++) {
    if(drones.get(i) == droneName) {
      String sentCommand = "CONNECT-REPLY";
      String sentToName = droneName;
      String sentDetails = "This drone already exists in list";
      bool sentRespone = false;
      
      Serial.println(sentDetails);
      while(!sendCommand(sentCommand, sentToName, sentDetails, sentResponse)) {
        //Keep sending the command until it's a success
      }
      return;
    }
  }

  //Task 2: If not, add it to the list
  drones.add(droneName);
  Serial.print("Succesfully added: ");
  Serial.println(droneName);

  String sentCommand = "CONNECT-REPLY";
  String sentToName = droneName;
  String sentDetails = "Succesfully added";
  bool sentRespone = true;
  
  while(!sendCommand(sentCommand, sentToName, sentDetails, sentResponse)) {
    //Keep sending the command until it's a success
  }
  return;
}

bool sendCommand(String sentCommand, String sentToName, String sentDetails, bool sentResponse) {
  Serial.print("sentCommand: ");
  Serial.println(sentCommand);

  Serial.print("sentToName: ");
  Serial.println(sentToName);

  Serial.print("sentDetails: ");
  Serial.println(sentDetails);

  Serial.print("sentResponse: ");
  Serial.println(sentResponse);

  sent["command"] = sentCommand;
  sent["toName"] = sentToName;
  sent["fromName"] = myName;
  sent["details"] = sentDetails;
  sent["response"] = sentResponse;
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

bool receiveCommand() {
  if(HC12.available()) {
    DeserializationError err = deserializeJson(received, HC12);
    if(err == DeserializationError::Ok) {
      Serial.println("Received a clean command...");

      String receivedCommand = received["command"].as<String>();
      String receivedToName = received["toName"].as<String>();
      String receivedFromName = received["fromName"].as<String>();
      String receivedDetails = received["details"].as<String>();
      bool receivedResponse = received["response"].as<bool>();

      Serial.print("receivedCommand: ");
      Serial.println(receivedCommand);
    
      Serial.print("receivedToName: ");
      Serial.println(receivedToName);
    
      Serial.print("receivedFromName: ");
      Serial.println(receivedFromName);

      Serial.print("receivedDetails: ");
      Serial.println(receivedDetails);
    
      Serial.print("receivedResponse: ");
      Serial.println(receivedResponse);

      if(receivedCommand == "REPLY" && receivedToName == myName) {
        return response;
      } else if(receivedCommand == "CONNECT" && receivedToName == myName) {
        addDrone(receivedFromName);
        return response;
      } else {
        Serial.print("The command '");
        Serial.print(receivedCommand);
        Serial.println("' is not recognized.");
        return false;
      }
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
