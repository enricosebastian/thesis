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
const int rxPin = A4;
const int txPin = A5;

// Waiting times
const int waitingTime = 5000;

// Only for base station
const int redLed = 13;
const int yellowLed = 12;
const int greenLed = 11;

const float distanceBetweenTags = 9.5;

//Booleans for logic
bool isConnected = false;
bool isDeployed = false;
bool hasStopped = true;
bool hasDetectedObject = false;

//millis time variables for storage
unsigned long startTime = 0;
unsigned long startTime2 = 0;

float currentX = 0;
float homeX = 0;
float savedX = 0;

float currentY = 0;
float homeY = 0;
float savedY = 0;

float d1 = 0;
float d2 = 0;

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
NeoSWSerial Esp(rxPin, txPin);

LinkedList<String> drones;

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);
  Nano.begin(9600);
  Esp.begin(9600);

  Serial.print(myName);
  Serial.println(" is initializing...");
  
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
  Serial.println(" has initialized.");
  startTime = millis();
  startTime2 = millis();
}

void loop() {
  if(myName == "BASE") {
    forBaseStation();
  } else {
    forDrone();
  }
}

void forBaseStation() {
  //STATE 1: If you are not yet deploying, capture all the drones that want to connect with you.
  if(!isDeployed) {

    //TASK 1 Waiting for a drone to connect. Add it to list
    if(receivedSpecificCommand("CONN")) {
      Serial.print(receivedFromName);
      Serial.println(" wanted to connect. Sending handshake.");
      startTime = millis();
      startTime2 = millis();
      while(millis() - startTime < waitingTime) {
        if(millis() - startTime2 >= 800) {
          startTime2 = millis();
          sendCommand("CONNREP", receivedFromName, "SUCC");
        }
      }
      addDrone(receivedFromName);

      //Count drones
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

    //TASK 2 If you pressed the button, deploy all drones
    if(digitalRead(btn) == HIGH && drones.size() > 0) {
      Serial.println("Button pressed. Starting deployment.");
      digitalWrite(redLed, LOW);
      digitalWrite(yellowLed, HIGH);
      digitalWrite(greenLed, LOW);

      for(int i = 0; i < drones.size(); i++) {
        //Reset values first 
        receivedFromName = "";
        receivedCommand = "";
        receivedToName = "";
        receivedDetails = "";

        Serial.print("Deploying: ");
        Serial.println(drones.get(i));
        startTime = millis();

        sendCommand("DEPL", drones.get(i), "HELL");
        while(!receivedSpecificCommand("DEPLREP") && receivedFromName != drones.get(i)) {
          if(millis() - startTime > waitingTime) {
            startTime = millis();

            Serial.print("Did not receive 'DEPLREP' from '");
            Serial.print(drones.get(i));
            Serial.println("' yet. Sending 'DEPL' again.");
            sendCommand("DEPL", drones.get(i), "HELL");
          }
        }
        Serial.print("Successfully deployed: ");
        Serial.println(drones.get(i));
      }

      isDeployed = true;

      Serial.println("All drones have been deployed. You can start sending commands now.");

      digitalWrite(redLed, LOW);
      digitalWrite(yellowLed, LOW);
      digitalWrite(greenLed, HIGH);
      digitalWrite(detectionPin, HIGH);
    } else if(digitalRead(btn) == HIGH && drones.size() < 1) {
      Serial.println("No drones to deploy.");
    }
  }

  //STATE 2: Base station has deployed everyone. Read and send commands
  if(isDeployed) {
    //TASK 1: If you typed something, send it to base station
    if(millis() - startTime > 800) {
      digitalWrite(redLed, LOW);
      digitalWrite(yellowLed, LOW);
      digitalWrite(greenLed, !digitalRead(greenLed));

      startTime = millis();
    }
    
    while(Serial.available()) {
      char letter = Serial.read();

      if(letter == '\n') {
        sentMessage += '\n';
        Serial.print("final: ");
        Serial.println(sentMessage);

        int endIndex = sentMessage.indexOf(' ');
        sentCommand = sentMessage.substring(0, endIndex);
        sentMessage = sentMessage.substring(endIndex+1);

        endIndex = sentMessage.indexOf(' ');
        sentToName = sentMessage.substring(0, endIndex);
        sentDetails = sentMessage.substring(endIndex+1);
        sentMessage = "";

        startTime = millis();
        sendCommand(sentCommand, sentToName, sentDetails);

        while(!receivedSpecificCommand(sentCommand+"REP")) {
          if(millis() - startTime > 800) {
            Serial.print(sentCommand);
            Serial.println("REP was not received yet. Resending command.");
            sendCommand(sentCommand, sentToName, sentDetails);
            startTime = millis();            
          }

          if(Serial.available() && Serial.readStringUntil('\n') != "wakaflaka") {
            Serial.println("Canceling sending of command. Try again.");
            break;
          }
        }        
      } else {
        sentMessage += letter;
      }
    }

    //TASK 2: Wait for base station to send me a command
    if(receiveCommand()) {      
    }
  }
}

void forDrone() {
  //STATE 1: Not connected to base station
  if(!isConnected && !isDeployed) {
    //TASK 1: Continue to wait for connection acknowledgement
    while(!receivedSpecificCommand("CONNREP")) {
      if(millis() - startTime > 800) {
        Serial.println("Reply 'CONNREP' was not received. Resending message again.");
        sendCommand("CONN", "BASE", "HELL");
        startTime = millis();        
      }
    }
    isConnected = true;
    Serial.println("Successfully detected by base station. Waiting for deployment.");
    sendToNano("CONN", myName, "SUCC");
  }

  //STATE 2: Connected, but waiting for deployment
  if(isConnected && !isDeployed) {
    //TASK 1: Continue to wait for deployment command
    while(!receivedSpecificCommand("DEPL")) {
      if(millis() - startTime > waitingTime) {
        Serial.println("Command 'DEPL' was not received yet. Continue waiting.");
        startTime = millis();        
      }

      // Task 3: Update coordinates
      if(millis() - startTime > 500) {
        HC12.end();
        Esp.listen();
        startTime2 = millis();
        while(millis() - startTime2 < 1000) {
          if(receiveCommand() && receivedCommand == "COOR") {
            int endIndex = receivedDetails.indexOf(',');
            d1 = receivedDetails.substring(0, endIndex).toFloat();
            d2 = receivedDetails.substring(endIndex+1).toFloat();

            currentX = (distanceBetweenTags*distanceBetweenTags - d2*d2 + d1*d1)/(2*distanceBetweenTags);
            currentY = sqrt(abs(d1*d1 - currentX*currentX));
            Serial.print("X,Y: ");
            Serial.print(currentX);
            Serial.print(",");
            Serial.println(currentY);
          }
        }
        Esp.end();
        HC12.listen();
        startTime = millis();
        startTime2 = millis();
        if(receivedCommand == "COOR") sendToNano(receivedCommand, receivedToName, receivedDetails); 
      }
    }

    Serial.println("Base station wants to start deploying. Sending acknowledgement.");
    startTime = millis();
    startTime2 = millis();

    homeX = currentX;
    homeY = currentY;

    while(millis() - startTime < waitingTime) {
      if(millis() - startTime2 > 800) {
        sendCommand("DEPLREP", receivedFromName, "SUCC");
        startTime2 = millis();
      }
    }
    
    isDeployed = true;
    hasStopped = true;
    startTime = millis();

    receivedDetails = String(homeX) + "," String(homeY);

    Serial.print("Drone is deploying. Home coordinate is: ");
    Serial.println(receivedDetails);
    sendToNano("DEPL", myName, receivedDetails);
  }

  //STATE 3: Drone is deployed. Move, receive commands, send commands, and detect objects
  if(isConnected && isDeployed) {

    // Task 1: Interpret commands
    while(receiveCommand()) {

      //Send acknowledgement that we received the command first
      startTime = millis();
      startTime2 = millis();
      while(millis() - startTime < waitingTime) {
        if(millis() - startTime2 > 800) {
          sendCommand(receivedCommand+"REP", receivedFromName, "SUCC");
          startTime2 = millis();
        }
      }
      //Interpret the command
      if(receivedCommand == "STOP") {
        hasStopped = true;
        sendToNano(receivedCommand, myName, receivedDetails);
        digitalWrite(detectionPin, LOW); // Turn off camera
        digitalWrite(recordingPin, LOW);
      } else if(receivedCommand == "GO") {
        hasStopped = false;
        savedX = currentX;
        savedY = currentY;

        receivedDetails = String(savedX) + "," String(savedY);

        sendToNano(receivedCommand, myName, receivedDetails);
        digitalWrite(detectionPin, HIGH); // Turn on camera
        digitalWrite(recordingPin, LOW);
      } else if(receivedCommand == "RECO") {
        hasStopped = false;
        sendToNano("GO", myName, receivedDetails);
        digitalWrite(detectionPin, LOW);
        digitalWrite(recordingPin, HIGH);
      } else if(receivedCommand == "WHER") {
        if(receivedDetails == "HOME") {
          sentDetails = String(homeX) + "," + String(homeY); // Send home coordinates
        } else if(receivedDetails == "CURR") {
          sentDetails = String(currentX) + "," + String(currentY); // Send current coordinates
        } else if(receivedDetails == "SAVE") {
          sentDetails = String(savedX) + "," + String(savedY); // Send saved coordinates
        }
        startTime = millis();
        while(millis() - startTime < 500) {
          sendCommand("HOME", "BASE", sentDetails);
        }
      } else {
        sendToNano(receivedCommand, myName, receivedDetails);
      }
    }

    // Task 2: If serial is available, you detected an object...
    while(Serial.available()) {
      char letter = Serial.read();
      if(letter == '\n') {
        receivedMessage += '\n';
        Serial.print("final: ");
        Serial.println(receivedMessage);
        
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
        if(receivedCommand == "DETE") {
          if(receivedDetails == "DONE") {
            hasDetectedObject = false;
          } else {
            hasDetectedObject = true;
          }
          sendToNano(receivedCommand, myName, receivedDetails);
        }
      } else {
        receivedMessage += letter;
      }
    }

    // Task 3: Update coordinates
    if(millis() - startTime > 500) {
      HC12.end();
      Esp.listen();
      startTime2 = millis();
      while(millis() - startTime2 < 1000) {
        if(receiveCommand() && receivedCommand == "COOR") {
          int endIndex = receivedDetails.indexOf(',');
          d1 = receivedDetails.substring(0, endIndex).toFloat();
          d2 = receivedDetails.substring(endIndex+1).toFloat();

          currentX = (distanceBetweenTags*distanceBetweenTags - d2*d2 + d1*d1)/(2*distanceBetweenTags);
          currentY = sqrt(abs(d1*d1 - currentX*currentX));
          Serial.print("X,Y: ");
          Serial.print(currentX);
          Serial.print(",");
          Serial.println(currentY);
        }
      }
      Esp.end();
      HC12.listen();
      startTime = millis();
      startTime2 = millis();
      if(receivedCommand == "COOR") sendToNano(receivedCommand, receivedToName, receivedDetails); 
    }
  }
}

///////Specific functions/////////
void sendToNano(String command, String toName, String details) {
  HC12.end();
  Nano.listen();

  //COMMAND TONAME FROMNAME DETAILS
  if(command != "" && toName != "" && details != "") {
    String sentMessage = command + " " + toName + " " + myName + " " + details;
    String bufferMessage = "BUFF " + toName + " " + myName + " " + "BUFF";
    Serial.print("Sending to Nano: ");
    Serial.println(sentMessage);
    
    Nano.println(bufferMessage); //Ned to send a buffer message first before sending actual message to clear port
    Nano.println(sentMessage);
  } else {
    Serial.println("Wrong format of command. Try again.");
  }
  Nano.end();
  HC12.listen();
}

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
}

///////General functions/////////
bool receiveCommand() {
  while(HC12.available()) {
    char letter = HC12.read();
    if(letter == '\n') {
      receivedMessage += '\n';
      Serial.print("Received: ");
      Serial.print(receivedMessage);

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
    String sentMessage = command + " " + toName + " " + myName + " " + details;
    HC12.println(sentMessage);
  } else {
    Serial.println("Wrong format of command. Try again.");
  }
}

bool receivedSpecificCommand(String command) {
  return receiveCommand() && (receivedCommand == command);
}
