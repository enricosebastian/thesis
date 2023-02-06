#include <Servo.h>
#include <Wire.h>
#include <HMC5883L_Simple.h>
#include <NeoSWSerial.h>

HMC5883L_Simple Compass;
/*
 * GY-273 Compass Module  ->  Arduino
 * VCC  -> VCC
 * GND  -> GND
 * SCL  -> A5, blue
 * SDA  -> A4, green
*/

//Name here
const String myName = "DRO1";
// const String myName = "DRO2";
// const String myName = "DRO3";

//Constants (buttons)
const int escLeftPin = 6;
const int escRightPin = 5;
const int txNano = 9; //green tx
const int rxNano = 8; //blue received
const int waitingTime = 5000;

//movement constants
const float minSpeed = 7;
const float movingSpeed = 15;
const float maxSpeed = 20;
const float maxAngleChange = 5;

//Booleans for logic


//Variables
int posX = 0;
int posY = 0;

float initialAngle = 0;
float kp = 8;
float ki = 0.2;
float kd = 30;
float PID_p, PID_i, PID_d, PID_total;

//received message
String receivedMessage = "";
String receivedCommand = "";
String receivedToName = "";
String receivedFromName = "";
String receivedDetails = "";

Servo escLeft;
Servo escRight;

NeoSWSerial Nano(txNano, rxNano); // (Green TX, Blue RX)

void setup() {
  Serial.begin(9600);
  Nano.begin(9600);

  Nano.listen();

  //Compass initialization
  Wire.begin();
  Compass.SetDeclination(-2, 37, 'W');  
  Compass.SetSamplingMode(COMPASS_SINGLE);
  Compass.SetScale(COMPASS_SCALE_130);
  Compass.SetOrientation(COMPASS_HORIZONTAL_X_NORTH);

  //ESC initialization
  escLeft.attach(escLeftPin,1000,2000);
  escRight.attach(escRightPin,1000,2000);
  escLeft.write(0);
  escRight.write(0);

  Serial.print(myName);
  Serial.println(" - Nano has initialized.");
}

void loop() {
  if(receiveCommand()) {
    //do nothing
  }
}

///////General functions/////////
bool receiveCommand() {
  while(Nano.available()) {
    char letter = Nano.read();
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
    Nano.println(sentMessage);
  } else {
    Serial.println("Wrong format of command. Try again.");
  }
}

bool receivedSpecificCommand(String command) {
  return receiveCommand() && (receivedCommand == command);
}