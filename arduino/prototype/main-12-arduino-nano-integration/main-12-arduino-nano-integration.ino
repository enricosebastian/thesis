#include <Arduino.h>
#include <NeoSWSerial.h>
#include <LinkedList.h>

//Name here
// const String myName = "BASE";
const String myName = "DRO1";
// const String myName = "DRO2";
// const String myName = "DRO3";

//Constants (buttons)
const int redLed = 13;
const int yellowLed = 12;
const int greenLed = 11;
const int detectionPin = 10;
const int btn = 7;
const int txHc12 = A0; //green tx
const int rxHc12 = A1; //blue received
const int txNano = A2; //green tx
const int rxNano = A3; //blue received
const int waitingTime = 5000;

//Booleans for logic
bool isConnected = false;
bool isDeployed = false;
bool hasStopped = false;
bool hasReceivedCommand = false;
bool hasDetectedObject = false;

//millis time variables for storage
unsigned long startTime = 0;
unsigned long startTime2 = 0;

//Variables
int posX = 0;
int posY = 0;

//received message
String receivedMessage = "";
String receivedCommand = "";
String receivedToName = "";
String receivedFromName = "";
String receivedDetails = "";

NeoSWSerial HC12(txHc12, rxHc12); // (Green TX, Blue RX)
NeoSWSerial Nano(txNano, rxNano); // (Green TX, Blue RX)
LinkedList<String> drones;

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);
  Nano.begin(9600);
  Serial.print(myName);
  Serial.println(" is initializing...");
  
  //GPIO initialization
  pinMode(redLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(detectionPin, OUTPUT);
  pinMode(btn, INPUT);

  //Turn on all lights first to signify that connections are safely grounded
  digitalWrite(redLed, HIGH);
  digitalWrite(yellowLed, HIGH);
  digitalWrite(greenLed, HIGH);

  //Set output pins
  digitalWrite(detectionPin, LOW);

  //Successful intialization indicator
  if(myName == "BASE") {
    //For base station, no LED should turn on
    digitalWrite(redLed, LOW);
    digitalWrite(yellowLed, LOW);
    digitalWrite(greenLed, LOW);
  } else {
    //For drone, it should be on a red LED
    digitalWrite(redLed, HIGH);
    digitalWrite(yellowLed, LOW);
    digitalWrite(greenLed, LOW);
  }

  HC12.listen();

  Serial.print(myName);
  Serial.println(" has initialized.");
  startTime = millis();
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

    //TASK 1 Count all drones
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

    //TASK 2 Waiting for a drone to connect. Add it to list
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
    }

    //TASK 3 If you pressed the button, deploy all drones
    if(digitalRead(btn) == HIGH) {
      Serial.println("Button pressed. Starting deployment.");

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
      digitalWrite(yellowLed, HIGH);
      digitalWrite(greenLed, LOW);

      digitalWrite(detectionPin, HIGH);
    }
  }

  //STATE 2: Base station has deployed everyone. Read and send commands
  if(isDeployed) {

    //TASK 1: If you typed something, send it to base station
    String sentMessage = "";
    String sentCommand = "";
    String sentToName = "";
    String sentFromName = "";
    String sentDetails = "";

    while(Serial.available()) {
      char letter = Serial.read();
      if(letter == '\n') {
        sentMessage += '\n';
        Serial.print("Sending: ");
        Serial.print(sentMessage);

        int endIndex = sentMessage.indexOf(' ');
        sentCommand = sentMessage.substring(0, endIndex);
        sentMessage = sentMessage.substring(endIndex+1);

        endIndex = sentMessage.indexOf(' ');
        sentToName = sentMessage.substring(0, endIndex);
        sentDetails = sentMessage.substring(endIndex+1);
        sentMessage = ""; // Erase old message

        startTime = millis();
        sendCommand(sentCommand, sentToName, sentDetails);
        while(!receivedSpecificCommand(sentCommand+"REP")) {
          if(millis() - startTime > 800) {
            Serial.print(command);
            Serial.println("REP was not yet received. Resending commmand.");
            sendCommand(sentCommand, sentToName, sentDetails);
            startTime = millis();
          }

          if(Serial.available() && Serial.readStringUntil('\n') != "smnth") {
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
  //TASK 1: Keep sending connect command until acknowledged.
  if(!isConnected && !isDeployed) {
    if(!receivedSpecificCommand("CONNREP")) {
      if(millis() - startTime >= waitingTime) {
        Serial.println("Reply 'CONNREP' was not received. Resending message again.");
        sendCommand("CONN", "BASE", "HELL");
        startTime = millis();
      }
    } else {
      Serial.println("Successfully detected by base station. Now we wait for deployment.");
      isConnected = true;
    }
  }

  //TASK 2: Wait for base station to send deploy command to start moving.
  if(isConnected && !isAcknowledging && !isDeployed) {
    if(!receivedSpecificCommand("DEPL")) {
      if(millis() - startTime >= waitingTime) {
        Serial.println("Command 'DEPL' was not received yet. Continue waiting.");
        startTime = millis();
      }
    } else {
      Serial.println("Base station wants to start deploying.");
      isAcknowledging = true;
      startTime = millis();
      startTime2 = millis();
      moveDrone("DEPL", myName, "SUCC");
    }
    digitalWrite(redLed, LOW);
    digitalWrite(yellowLed, HIGH);
    digitalWrite(greenLed, LOW);
  }

  //TASK 2.1: Base station wants to deploy us. Send acknowledgement/handshake for at least 5 seconds
  if(isConnected && isAcknowledging && !isDeployed) {
    if(millis() - startTime <= waitingTime) {
      if(millis() - startTime2 >= 800) {
        sendCommand("DEPLREP", receivedFromName, "SUCC");
        startTime2 = millis();
      }
    } else if(millis() - startTime > waitingTime) {
      digitalWrite(detectionPin, HIGH);
      Serial.println("Drone is deploying. Moving motors.");
      isAcknowledging = false;
      isDeployed = true;
      startTime = millis();
    }
  }
  
  //TASK 3: Start moving. Plus, look for commands from base station. And also RPi.
  if(isConnected && !isAcknowledging && isDeployed) {

    //TASK 3.1: If you received a command from base station, stop what you are doing and interpret the command.
    if(!hasDetectedObject && !hasReceivedCommand && receiveCommand()) {
      hasReceivedCommand = true;
      startTime = millis();
    }

    //TASK 3.1.2: Read what each command means
    if(!hasDetectedObject && hasReceivedCommand) {
      if(millis() - startTime <= waitingTime) {
        if(millis() - startTime2 > 800) {
          sendCommand(receivedCommand+"REP", receivedFromName, "SUCC"); // Send acknowledgement that you received a command
          startTime2 = millis();
        }
      } else if(millis() - startTime > waitingTime) {
        // Turn off hasReceivedCommand, and then interpret the actual command
        hasReceivedCommand = false;
        
        if(receivedCommand == "STOP") {
          Serial.println("Stopping drone.");
          digitalWrite(redLed, HIGH);
          digitalWrite(yellowLed, LOW);
          digitalWrite(greenLed, LOW);
          
          hasStopped = true;
          moveDrone("STOP", myName, "SUCC");
        } else if(receivedCommand == "GO") {
          Serial.println("Drone resuming deployment.");
          digitalWrite(redLed, LOW);
          digitalWrite(yellowLed, LOW);
          digitalWrite(greenLed, HIGH);
          
          hasStopped = false; // Revert status back to false
          moveDrone("GO", myName, "SUCC");
        } else if(receivedCommand == "TURN") {
          moveDrone("TURN", myName, receivedDetails);
        }
      }
    }

    //TASK 3.2: Ensure that you are moving
    if(!hasDetectedObject && !hasReceivedCommand && !hasStopped) {
      if(millis() - startTime >= 1000) {
        startTime = millis();
        digitalWrite(redLed, LOW);
        digitalWrite(yellowLed, LOW);
        digitalWrite(greenLed, !digitalRead(greenLed));
      }
    }
    
    //TASK 3.3: Look for RPi commands (Check if you detect an object in the water)
    if(!hasDetectedObject && !hasReceivedCommand && !hasStopped && detectObject()) {
      hasDetectedObject = true;
      startTime = millis();
    }
    
    if(!hasReceivedCommand && hasDetectedObject && !hasStopped) {
      //Basically deconstruct the details of the object and its location here
      if(receivedDetails == "LEFT") {
        moveDrone("DETE", myName, receivedDetails);
      } else if(receivedDetails == "CENTER") {
        moveDrone("DETE", myName, receivedDetails);
      } else if(receivedDetails == "RIGHT") {
        moveDrone("DETE", myName, receivedDetails);
      }
    }
    
    //Task 3.4: If command says to stop, then stop the prototype.
    if(!hasReceivedCommand && !hasDetectedObject && hasStopped) {
      //Do nothing lmao. Go home
    }
  }
}

///////Specific functions/////////
void moveDrone(String command, String toName, String details) {
  //COMMAND TONAME FROMNAME DETAILS
  if(command != "" && toName != "" && details != "") {
    String sentMessage = command + " " + toName + " " + myName + " " + details;
    Serial.print("Sending: ");
    Serial.println(sentMessage);
    Nano.println(sentMessage);
  } else {
    Serial.println("Wrong format of command. Try again.");
  }
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
      return (receivedCommand != "") && (receivedToName == myName) && (receivedFromName != "") && (receivedDetails != "");
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
