// Ubuntu command line for serial port: sudo chmod a+rw /dev/ttyACM0
#include <Arduino.h>
#include <NeoSWSerial.h>
#include <LinkedList.h>

//Name here
// const String myName = "BASE";
const String myName = "DRO1";
// const String myName = "DRO2";
// const String myName = "DRO3";

//Constants (buttons)
const int detectionPin = 10;
const int recordingPin = 9;
const int btn = 7;
const int coordPin = 6;

// Rule: For ports, green = RX, blue = TX
// For modules/chips, green = TX, blue = RX
const int rxHc12 = A0; //green wire
const int txHc12 = A1; //blue wire
const int rxNano = A2; //green wire
const int txNano = A3; //blue wire
const int rxEsp = A4;
const int txEsp = A5;

// Waiting times
const int waitingTime = 5000;

// Only for base station
const int redLed = 13;
const int yellowLed = 12;
const int greenLed = 11;

//Booleans for logic
bool isConnected = false;
bool isDeployed = false;
bool hasStopped = true;
bool hasDetectedObject = false;

//millis time variables for storage
unsigned long startTime = 0;
unsigned long startTime2 = 0;

//received message
String receivedMessage = "";
String receivedCommand = "";
String receivedToName = "";
String receivedFromName = "";
String receivedDetails = "";

//sent message
String sentMessage = "";
String sentCommand = "";
String sentToName = "";
String sentFromName = "";
String sentDetails = "";

// SoftwareSerial(rxPin, txPin, inverse_logic)
NeoSWSerial HC12(rxHc12, txHc12);
NeoSWSerial Nano(rxNano, txNano);
NeoSWSerial Esp(rxEsp, txEsp);

LinkedList<String> drones;

void setup() {
  // Actual serial
  Serial.begin(9600);

  // Software serial
  HC12.begin(9600);
  Nano.begin(9600);
  Esp.begin(9600);
  
  //GPIO initialization
  pinMode(detectionPin, OUTPUT);
  pinMode(recordingPin, OUTPUT);
  pinMode(btn, INPUT);

  //Set output pins
  digitalWrite(detectionPin, LOW);
  digitalWrite(recordingPin, LOW);

  //Successful intialization indicator
  if(myName == "BASE") {
    pinMode(redLed, OUTPUT);
    pinMode(yellowLed, OUTPUT);
    pinMode(greenLed, OUTPUT);

    //Turn on all lights first to signify that connections are safely grounded
    digitalWrite(redLed, HIGH);
    digitalWrite(yellowLed, HIGH);
    digitalWrite(greenLed, HIGH);

    delay(1000);

    //For base station, no LED should turn on
    digitalWrite(redLed, LOW);
    digitalWrite(yellowLed, LOW);
    digitalWrite(greenLed, LOW);
  }

  HC12.listen();

  Serial.print(myName);
  Serial.println("v16 is finished initializing.");

  startTime = millis();
  startTime2 = millis();
}

void loop() {
  // State 1: Check if you received anything from base station
  if(hasReceivedMessage() && !hasSentSerially()) {
    Serial.print("Received '");
    Serial.print(receivedCommand);
    Serial.print("' from ");
    Serial.print(receivedFromName);
    Serial.print(". Details: ");
    Serial.println(receivedDetails);

    // Step 1: Successful receive warrants an acknowledgement
    startTime = millis();
    while(millis() - startTime < 1000) {
      if(millis() - startTime2 > 300) {
        if(receivedCommand == "GREE") {
          receivedDetails = "Hello!";
        }        
        sendCommand(receivedCommand+"REP", receivedFromName, receivedDetails);
        startTime2 = millis();
      }
    }

    // Step 2: Interpret command
    if(receivedCommand == "GREE") {
      Serial.print(receivedFromName);
      Serial.println(" wants us to say hi.");
    } else if(receivedCommand == "CONN" && !isDeployed && myName == "BASE") {
      Serial.print("Drone '");
      Serial.print(receivedFromName);
      Serial.println("' wants to connect.");
      addDrone(receivedFromName);
    } else if(receivedCommand == "CONNREP" && !isConnected && myName != "BASE") {
      isConnected = true;

      Serial.print(myName);
      Serial.println(" is now connected. Ready for deployment.");
    } else if(receivedCommand == "DEPL" && isConnected && myName != "BASE") {
      isDeployed = true;

      Serial.print(myName);
      Serial.println(" is now deployed.");
    }
  }

  // State 2: Check if user is sending anything to base station
  if(hasSentSerially() && !hasReceivedMessage()) {
    Serial.print("Sent '");
    Serial.print(sentCommand);
    Serial.print("' to ");
    Serial.println(sentToName);

    // Step 1: Wait for acknowledgement
    sendCommand(sentCommand, sentToName, sentDetails);
    startTime = millis();
    while(!hasReceivedCommand(sentCommand+"REP")) {
      if(millis() - startTime > 300) {
        Serial.print("'");
        Serial.print(sentCommand);
        Serial.println("' was not received. Sending command again.");
        sendCommand(sentCommand, sentToName, sentDetails);
        startTime = millis();
      }

      if(Serial.available() && Serial.read() == 'c') {
        Serial.print("User canceled sending of '");
        Serial.print(sentCommand);
        Serial.println("'");
        break;
      }
    }
  }

  if(myName == "BASE") {
    forBaseStation();
  } else {
    forDrone();
  }
}

void forBaseStation() {
  if(!isDeployed) {
    if(digitalRead(btn) == HIGH && drones.size() > 0) {
      isDeployed = true;
      Serial.println("Deploying all drones.");

      for(int i = 0; i < drones.size(); i++) {
        //Reset values first 
        receivedFromName = "";
        receivedCommand = "";
        receivedToName = "";
        receivedDetails = "";

        Serial.print("Deploying ");
        Serial.println(drones.get(i));

        startTime = millis();
        sendCommand("DEPL", drones.get(i), "YEET");
        while(!hasReceivedCommand("DEPLREP") && receivedFromName != drones.get(i)) {
          if(millis() - startTime > 500) {
            Serial.print("Did not receive DEPLREP from");
            Serial.print(drones.get(i));
            Serial.println(". Sending DEPL again.");
            sendCommand("DEPL", drones.get(i), "YEET");
            startTime = millis();
          }
        }
        Serial.print("Successfully deployed ");
        Serial.println(drones.get(i));
      }

      isDeployed = true;
      Serial.println("All drones have been deployed.");    
    } else if(digitalRead(btn) && drones.size() <= 0) {
      Serial.println("No drones to deploy.");
    }
  }
  
}

void forDrone() {
  if(!isConnected && !isDeployed) {
    if(millis() - startTime > 500) {
      sendCommand("CONN", "BASE", "PLES");
      startTime = millis();
    }
  }

  if(isConnected && !isDeployed) {
    // wait for depl
  }

  if(isConnected && isDeployed) {
    // move
  }
}

//////Specific functions/////////
void addDrone(String droneName) {
  //Check first if drone already exists in list
  for(int i = 0; i < drones.size(); i++) {
    if(drones.get(i) == droneName) {
      Serial.print(droneName);
      Serial.println(" is already connected!");
      return;
    }
  }
  drones.add(droneName); //Successfully added this drone.
  Serial.print(droneName);
  Serial.println(" has been added.");
}

///////General functions/////////
bool hasReceivedMessage() {
  while(HC12.available()) {
    char letter = HC12.read();

    if(letter == '\n') {
      int endIndex = receivedMessage.indexOf(' ');
      receivedCommand = receivedMessage.substring(0, endIndex);
      receivedMessage = receivedMessage.substring(endIndex+1);

      endIndex = receivedMessage.indexOf(' ');
      receivedToName = receivedMessage.substring(0, endIndex);
      receivedMessage = receivedMessage.substring(endIndex+1);

      endIndex = receivedMessage.indexOf(' ');
      receivedFromName = receivedMessage.substring(0, endIndex);
      receivedDetails = receivedMessage.substring(endIndex+1);

      receivedMessage = ""; // Erase old message
      return (receivedCommand != "") && (receivedToName == myName || receivedToName == "ALL") && (receivedFromName != "") && (receivedDetails != "");
    } else {
      receivedMessage += letter;
    }
  }

  return false;
}

void sendCommand(String command, String toName, String details) {
  //COMMAND TONAME FROMNAME DETAILS
  if(command != "" && toName != "" && details != "") {
    sentMessage = command + " " + toName + " " + myName + " " + details;
    HC12.println(sentMessage);
  } else {
    Serial.println("Wrong format of command. Try again.");
  }

  sentMessage = ""; // Clears the message after sending
}

bool hasReceivedCommand(String command) {
  return hasReceivedMessage() && (receivedCommand == command);
}

bool hasSentSerially() {
  while(Serial.available()) {
    char letter = Serial.read();

    if(letter == '\n') {
      int endIndex = sentMessage.indexOf(' ');
      sentCommand = sentMessage.substring(0, endIndex);
      sentMessage = sentMessage.substring(endIndex+1);

      endIndex = sentMessage.indexOf(' ');
      sentToName = sentMessage.substring(0, endIndex);
      sentDetails = sentMessage.substring(endIndex+1);

      sentMessage = ""; // Erase old message
      return (sentCommand != "") && (sentToName != "") && (sentDetails != "");
    } else {
      sentMessage += letter;
    }
  }

  return false;
}