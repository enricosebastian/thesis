#include <ArduinoJson.h>
#include <SoftwareSerial.h>

SoftwareSerial HC12(6, 7); // HC-12 TX Pin, HC-12 RX Pin

StaticJsonDocument<200> sent;
StaticJsonDocument<200> received;

const String myName = "Drone1";
const int redLed = 13;
const int greenLed = 12;

bool isConnected = false;
bool isDeployed = false;

//int motorSpeedLeft = 0;
//int motorSpeedRight = 0;
//int allowance = 5;
//int escThreshold = 20;
//int controllerDelay = 0;
//float mainHeading = 0;
//float heading = 0;
//String currentCommand = "";
//unsigned long duration = 0;

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);
  Serial.println("Starting...");

  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);

  digitalWrite(greenLed, LOW);
  digitalWrite(redLed, LOW);
  
//  Compass.SetDeclination(-2, 37, 'W');
//  Compass.SetSamplingMode(COMPASS_SINGLE);
//  Compass.SetScale(COMPASS_SCALE_130);
//  Compass.SetOrientation(COMPASS_HORIZONTAL_X_NORTH);
//
//  escLeft.attach(10,1000,2000);
//  escRight.attach(11,1000,2000);
//  
//  escLeft.write(0);
//  escRight.write(0);
//  delay(2000);
  Serial.println("Initialization success. Looking for base station...");
}

void loop() {
  
  //Task 1: Establish connection with base station
  while(!isConnected) {
    connectToBaseStation();
  }

  //Task 2: Indicate that you are connected to base station
  Serial.println("Connected. Ready for deployment.");
  digitalWrite(greenLed, LOW);
  digitalWrite(redLed, HIGH);
  
  //Task 3: Wait for deployment
  while(!isDeployed) {
    receiveCommand();
  }

  //Task 4: Deploy. Do stuff.
  digitalWrite(greenLed, HIGH);
  digitalWrite(redLed, LOW);
  
  while(true) {
    Serial.println("Deployed!");
  }
}

void connectToBaseStation() {
  String sentCommand = "CONNECT";
  String sentToName = "BaseStation";
  String sentDetails = "Connecting to BaseStation";
  bool sentResponse = true;

  Serial.println(sentDetails);
  
  while(!sendCommand(sentCommand, sentToName, sentDetails, sentResponse)) {
    //Keep sending the command until it's a success
  }
  
  isConnected = true;
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
  while(!receiveCommand()) {
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
        return receivedResponse;
      } else if(receivedCommand == "CONNECT-REPLY" && receivedToName == myName) {
        return receivedResponse;
      } else if(receivedCommand == "DEPLOY" && receivedToName == myName) {
        isDeployed = true;
        return receivedResponse;
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
//  if(HC12.available()) {
//    DeserializationError err = deserializeJson(received, HC12);
//    
//    if(err == DeserializationError::Ok) {
//      String receivedCommand = received["command"].as<String>();
//      Serial.print("COMMAND RECEIVED: ");
//      Serial.println(receivedCommand);
//      
//      if(receivedCommand == "GO") {
//        currentCommand = "GO";
//        motorSpeedRight = received["motorSpeedRight"].as<int>();
//        motorSpeedLeft = received["motorSpeedLeft"].as<int>();
//        duration = received["duration"].as<unsigned long>();
//        controllerDelay = received["controllerDelay"].as<int>();
//        
//        mainHeading = Compass.GetHeadingDegrees();
//        Serial.print("Main heading: ");
//        Serial.println(mainHeading);
//        
//        Serial.print("Left: ");
//        Serial.println(motorSpeedLeft);
//  
//        Serial.print("Right: ");
//        Serial.println(motorSpeedRight);
//      } else if(receivedCommand == "STOP") {
//        currentCommand = "STOP";
//        motorSpeedLeft = 0;
//        motorSpeedRight = 0;
//      } else {
//        currentCommand = "";
//      }
//    }
//  }
//
//  if(currentCommand == "GO") {
//    heading = Compass.GetHeadingDegrees();
//    Serial.print("Original heading: ");
//    Serial.println(mainHeading);
//    Serial.print("Heading: \t");
//    Serial.println( heading );
//    Serial.println("\n");
//
//    if(heading < mainHeading-allowance) {
//      comment = "Too much left...";
//      if(motorSpeedLeft <= 0 || motorSpeedLeft < escThreshold) {
//        motorSpeedLeft += 1;
//      } else if(motorSpeedRight > 0 && motorSpeedLeft >= escThreshold) {
//        motorSpeedRight -= 1;
//      }
//    } else if (heading > mainHeading+allowance) {
//      comment = "Too much right...";
//      if(motorSpeedRight <= 0 || motorSpeedRight < escThreshold) {
//        motorSpeedRight += 1;
//      } else if(motorSpeedLeft > 0 && motorSpeedRight >= escThreshold) {
//        motorSpeedLeft -= 1;
//      }
//    } else {
//      comment = "All good...";
//    }
//    Serial.println(comment);
//    Serial.print("{");
//    Serial.print(motorSpeedLeft);
//    Serial.print(", ");
//    Serial.print(motorSpeedRight);
//    Serial.println("}");
//    Serial.println("\n");
//    
//    escLeft.write(motorSpeedLeft);
//    escRight.write(motorSpeedRight);
//    
//    sent["motorSpeedLeft"] = motorSpeedLeft;
//    sent["motorSpeedRight"] = motorSpeedRight;
//    sent["mainHeading"] = mainHeading;
//    sent["heading"] = heading;
//    sent["comment"] = comment;
//    serializeJson(sent, HC12);
//    
//    delay(controllerDelay);
//  } else if(currentCommand == "STOP") {
//    Serial.println("Stopped running...");
//    escLeft.write(0);
//    escRight.write(0);
//
//    sent["motorSpeedLeft"] = motorSpeedLeft;
//    sent["motorSpeedRight"] = motorSpeedRight;
//    sent["mainHeading"] = mainHeading;
//    sent["heading"] = heading;
//    sent["comment"] = "Stopped";
//    serializeJson(sent, HC12);
//  } else {
//    Serial.println("Null command...");
//    escLeft.write(0);
//    escRight.write(0);
//  }
//}
