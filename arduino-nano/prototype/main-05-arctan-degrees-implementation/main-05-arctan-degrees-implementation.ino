#include <NeoSWSerial.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <Servo.h>

//Name here
// const String myName = "DRO1";
const String myName = "DRO2";
// const String myName = "DRO3";

//Constants
const int greenLed = 7;
const int yellowLed = 8;
const int blueLed = 9;
const int redLed = 10;
const int escLeftPin = 5;
const int escRightPin = 6;

const int rxNano = 12; //blue [these need to be reversed (diff numeral order) cause tx1 --> rx2, and rx1 --> tx2]
const int txNano = 11; //green

const int waitingTime = 5000;
const int turnDelay = 30000; //in milliseconds

//movement constants
const float stopSpeed = 0;
const float movingSpeed = 15;
const float maxSpeed = 25;

const float angleAllowance = 5.0;

//Booleans for logic
bool isConnected = false;
bool isDeployed = false;
bool isCalibrated = false;
bool hasStopped = true;
bool hasDetectedObject = false;

//Variables
int posX = 0;
int posY = 0;
int savedDir = 0;

//PID values
float kp = 8;
float ki = 0.2;
float kd = 30;
float PID_p, PID_i, PID_d, PID_total;

float savedAngle = 0.0;

//millis time variables for storage
unsigned long startTime = 0;

//received message
String receivedMessage = "";
String receivedCommand = "";
String receivedToName = "";
String receivedFromName = "";
String receivedDetails = "";

Servo escLeft;
Servo escRight;

NeoSWSerial Nano(txNano, rxNano); // (Green TX, Blue RX)
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

String testName = "hi";

void setup() {
  Serial.begin(9600);
  Nano.begin(9600);

  Nano.listen();

  //ESC initialization
  escLeft.attach(escLeftPin,1000,2000);
  escRight.attach(escRightPin,1000,2000);
  escLeft.write(0);
  escRight.write(0);

  //HMC5883 initialization
  if(!mag.begin())
  {
    /* There was a problem detecting the HMC5883 ... check your connections */
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while(1);
  }

  //LED initialization
  pinMode(greenLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  pinMode(redLed, OUTPUT);

  digitalWrite(greenLed, HIGH);
  digitalWrite(yellowLed, HIGH);
  digitalWrite(blueLed, HIGH);
  digitalWrite(redLed, HIGH);

  delay(1000);

  digitalWrite(greenLed, LOW);
  digitalWrite(yellowLed, LOW);
  digitalWrite(blueLed, LOW);
  digitalWrite(redLed, HIGH);

  Serial.print(myName);
  Serial.println(" - Nano has initialized.");
}

void loop() {
  sensors_event_t event; 
  mag.getEvent(&event);
  
  // Main task: Continue to check for commands
  if(receiveCommand()) {
    if(receivedCommand == "CONN" && !isConnected) {
      digitalWrite(greenLed, LOW);
      digitalWrite(yellowLed, HIGH);
      digitalWrite(blueLed, LOW);
      digitalWrite(redLed, LOW);

      isConnected = true;
    } else if(receivedCommand == "DEPL") {
      isDeployed = true;
      hasStopped = true;
      startTime = millis();

      Serial.print(myName);
      Serial.println(" is deployed.");
    } else if(receivedCommand == "GO") {
      hasStopped = false;
      startTime = millis();

      Serial.print(myName);
      Serial.println(" is now moving.");

      savedHeadingX = currentHeadingX;
      savedHeadingY = currentHeadingY;
      Serial.print("Saved: ");
      Serial.print(savedHeadingX);
      Serial.print(", ");
      Serial.println(savedHeadingY);

      testName = getDirection(savedHeadingX, savedHeadingY);
      
      Serial.print(testName);
      Serial.println("is the saved angle");
    } else if(receivedCommand == "STOP") {
      hasStopped = true;
      startTime = millis();

      Serial.print(myName);
      Serial.println(" has stopped.");
      
      escLeft.write(stopSpeed);
      escRight.write(stopSpeed);
    } else if(receivedCommand == "DETE") {
      hasDetectedObject = true;
      if(receivedDetails == "DONE") {
        hasDetectedObject = false;
      }
    }
  }

  // State 1: Just connected to base station. Show current headings
  if(!isConnected && !isDeployed) {
    Serial.print(currentHeadingX);
    Serial.print(", ");
    Serial.println(currentHeadingY);
  }

  // State 2: When connected, not deployed, and needs calibration
  if(isConnected && !isCalibrated && !isDeployed) {
    calibrateCompass(currentHeadingX, currentHeadingY);
  }

  // State 2: When connected, not deployed, and needs calibration
  if(isConnected && isCalibrated && !isDeployed) {
    //demo directions
    displayDirection(currentHeadingX, currentHeadingY);
  }

  // State 3: Do all possible functions since you've been deployed
  if(isConnected && isCalibrated && isDeployed) {

    // State 1: Continuously moving
    if(!hasStopped && !hasDetectedObject) {
      // if(startTime - millis() > 1000) {
      //   digitalWrite(greenLed, !digitalRead(greenLed));
      //   digitalWrite(yellowLed, LOW);
      //   digitalWrite(blueLed, LOW);
      //   digitalWrite(redLed, LOW);

      //   startTime = millis();
      // }

      move(currentHeadingX, currentHeadingY);      
    }

    // State 2: Detected something, so move there
    if(!hasStopped && hasDetectedObject) {
      digitalWrite(greenLed, LOW);
      digitalWrite(yellowLed, LOW);
      digitalWrite(blueLed, HIGH);
      digitalWrite(redLed, LOW);
    }

    // State 3: Stop moving
    if(hasStopped) {

      if(startTime - millis() > 1000) {
        digitalWrite(greenLed, LOW);
        digitalWrite(yellowLed, LOW);
        digitalWrite(blueLed, LOW);
        digitalWrite(redLed, !digitalRead(redLed));

        startTime = millis();
      }
    }  

  }

}

///////Specific functions/////////
void calibrateCompass(float currentHeadingX, float currentHeadingY) {
  if(headingN_X == 6969.6969 || headingN_Y == 6969.6969) {
    Serial.println("Calibrating N: ");

    digitalWrite(greenLed, HIGH);
    digitalWrite(yellowLed, LOW);
    digitalWrite(blueLed, LOW);
    digitalWrite(redLed, LOW);

    delay(5000);

    digitalWrite(greenLed, LOW);
    digitalWrite(yellowLed, LOW);
    digitalWrite(blueLed, LOW);
    digitalWrite(redLed, LOW);

    headingN_X = currentHeadingX;
    headingN_Y = currentHeadingY;

    Serial.print(currentHeadingX);
    Serial.print(", ");
    Serial.println(currentHeadingY);
    
    delay(1000);
  } else if(headingNW_X == 6969.6969 || headingNW_Y == 6969.6969) {
    Serial.println("Calibrating NW: ");

    digitalWrite(greenLed, HIGH);
    digitalWrite(yellowLed, HIGH);
    digitalWrite(blueLed, LOW);
    digitalWrite(redLed, LOW);

    delay(5000);

    digitalWrite(greenLed, LOW);
    digitalWrite(yellowLed, LOW);
    digitalWrite(blueLed, LOW);
    digitalWrite(redLed, LOW);

    headingNW_X = currentHeadingX;
    headingNW_Y = currentHeadingY;

    Serial.print(currentHeadingX);
    Serial.print(", ");
    Serial.println(currentHeadingY);
    
    delay(1000);
  } else if(headingW_X == 6969.6969 || headingW_Y == 6969.6969) {
    Serial.println("Calibrating W: ");

    digitalWrite(greenLed, LOW);
    digitalWrite(yellowLed, HIGH);
    digitalWrite(blueLed, LOW);
    digitalWrite(redLed, LOW);

    delay(5000);

    digitalWrite(greenLed, LOW);
    digitalWrite(yellowLed, LOW);
    digitalWrite(blueLed, LOW);
    digitalWrite(redLed, LOW);

    headingW_X = currentHeadingX;
    headingW_Y = currentHeadingY;

    Serial.print(currentHeadingX);
    Serial.print(", ");
    Serial.println(currentHeadingY);
    
    delay(1000);
  } else if(headingSW_X == 6969.6969 || headingSW_Y == 6969.6969) {
    Serial.println("Calibrating SW: ");
    
    digitalWrite(greenLed, LOW);
    digitalWrite(yellowLed, HIGH);
    digitalWrite(blueLed, HIGH);
    digitalWrite(redLed, LOW);

    delay(5000);

    digitalWrite(greenLed, LOW);
    digitalWrite(yellowLed, LOW);
    digitalWrite(blueLed, LOW);
    digitalWrite(redLed, LOW);

    headingSW_X = currentHeadingX;
    headingSW_Y = currentHeadingY;

    Serial.print(currentHeadingX);
    Serial.print(", ");
    Serial.println(currentHeadingY);
    
    delay(1000);
  } else if(headingS_X == 6969.6969 || headingS_Y == 6969.6969) {
    Serial.println("Calibrating S: ");

    digitalWrite(greenLed, LOW);
    digitalWrite(yellowLed, LOW);
    digitalWrite(blueLed, HIGH);
    digitalWrite(redLed, LOW);

    delay(5000);

    digitalWrite(greenLed, LOW);
    digitalWrite(yellowLed, LOW);
    digitalWrite(blueLed, LOW);
    digitalWrite(redLed, LOW);

    headingS_X = currentHeadingX;
    headingS_Y = currentHeadingY;

    Serial.print(currentHeadingX);
    Serial.print(", ");
    Serial.println(currentHeadingY);
    
    delay(1000);
  } else if(headingSE_X == 6969.6969 || headingSE_Y == 6969.6969) {
    Serial.println("Calibrating SE: ");

    digitalWrite(greenLed, LOW);
    digitalWrite(yellowLed, LOW);
    digitalWrite(blueLed, HIGH);
    digitalWrite(redLed, HIGH);

    delay(5000);

    digitalWrite(greenLed, LOW);
    digitalWrite(yellowLed, LOW);
    digitalWrite(blueLed, LOW);
    digitalWrite(redLed, LOW);

    headingSE_X = currentHeadingX;
    headingSE_Y = currentHeadingY;

    Serial.print(currentHeadingX);
    Serial.print(", ");
    Serial.println(currentHeadingY);
    
    delay(1000);
  } else if(headingE_X == 6969.6969 || headingE_Y == 6969.6969) {
    Serial.println("Calibrating E: ");
    
    digitalWrite(greenLed, LOW);
    digitalWrite(yellowLed, LOW);
    digitalWrite(blueLed, LOW);
    digitalWrite(redLed, HIGH);

    delay(5000);

    digitalWrite(greenLed, LOW);
    digitalWrite(yellowLed, LOW);
    digitalWrite(blueLed, LOW);
    digitalWrite(redLed, LOW);

    headingE_X = currentHeadingX;
    headingE_Y = currentHeadingY;

    Serial.print(currentHeadingX);
    Serial.print(", ");
    Serial.println(currentHeadingY);
    
    delay(1000);
  } else if(headingNE_X == 6969.6969 || headingNE_Y == 6969.6969) {
    Serial.println("Calibrating NE: ");

    digitalWrite(greenLed, HIGH);
    digitalWrite(yellowLed, LOW);
    digitalWrite(blueLed, LOW);
    digitalWrite(redLed, HIGH);

    delay(5000);

    digitalWrite(greenLed, LOW);
    digitalWrite(yellowLed, LOW);
    digitalWrite(blueLed, LOW);
    digitalWrite(redLed, LOW);

    headingNE_X = currentHeadingX;
    headingNE_Y = currentHeadingY;

    Serial.print(currentHeadingX);
    Serial.print(", ");
    Serial.println(currentHeadingY);
    
    delay(1000);
  }

  isCalibrated = !(
    headingN_X == 6969.6969 ||
    headingE_X == 6969.6969 ||
    headingW_X == 6969.6969 ||
    headingS_X == 6969.6969 ||
    headingNE_X == 6969.6969 ||
    headingNW_X == 6969.6969 ||
    headingSE_X == 6969.6969 ||
    headingSW_X == 6969.6969 ||
    headingN_Y == 6969.6969 ||
    headingE_Y == 6969.6969 ||
    headingW_Y == 6969.6969 ||
    headingS_Y == 6969.6969 ||
    headingNE_Y == 6969.6969 ||
    headingNW_Y == 6969.6969 ||
    headingSE_Y == 6969.6969 ||
    headingSW_Y == 6969.6969
  );

  if(isCalibrated) startTime = millis();
}

String getDirection(float currentHeadingX, float currentHeadingY) {  
  if(
    currentHeadingX >= headingN_X-headingAllowance && 
    currentHeadingX <= headingN_X+headingAllowance &&
    currentHeadingY >= headingN_Y-headingAllowance && 
    currentHeadingY <= headingN_Y+headingAllowance
  ) return "N";
  else if(
    currentHeadingX > headingNW_X-(headingAllowance*headingAllowanceInBetween) && 
    currentHeadingX < headingNW_X+(headingAllowance*headingAllowanceInBetween) &&
    currentHeadingY > headingNW_Y-(headingAllowance*headingAllowanceInBetween) && 
    currentHeadingY < headingNW_Y+(headingAllowance*headingAllowanceInBetween)
  ) return "NW";
  else if(
    currentHeadingX >= headingW_X-headingAllowance && 
    currentHeadingX <= headingW_X+headingAllowance &&
    currentHeadingY >= headingW_Y-headingAllowance && 
    currentHeadingY <= headingW_Y+headingAllowance
  ) return "W";
  else if(
    currentHeadingX > headingSW_X-(headingAllowance*headingAllowanceInBetween) && 
    currentHeadingX < headingSW_X+(headingAllowance*headingAllowanceInBetween) &&
    currentHeadingY > headingSW_Y-(headingAllowance*headingAllowanceInBetween) && 
    currentHeadingY < headingSW_Y+(headingAllowance*headingAllowanceInBetween)
  ) return "SW";
  else if(
    currentHeadingX >= headingS_X-headingAllowance && 
    currentHeadingX <= headingS_X+headingAllowance &&
    currentHeadingY >= headingS_Y-headingAllowance && 
    currentHeadingY <= headingS_Y+headingAllowance
  ) return "S";
  else if(
    currentHeadingX > headingSE_X-(headingAllowance*headingAllowanceInBetween) && 
    currentHeadingX < headingSE_X+(headingAllowance*headingAllowanceInBetween) &&
    currentHeadingY > headingSE_Y-(headingAllowance*headingAllowanceInBetween) && 
    currentHeadingY < headingSE_Y+(headingAllowance*headingAllowanceInBetween)
  ) return "SE";
  else if(
    currentHeadingX >= headingE_X-headingAllowance && 
    currentHeadingX <= headingE_X+headingAllowance &&
    currentHeadingY >= headingE_Y-headingAllowance && 
    currentHeadingY <= headingE_Y+headingAllowance
  ) return "E";
  else if(
    currentHeadingX > headingNE_X-(headingAllowance*headingAllowanceInBetween) && 
    currentHeadingX < headingNE_X+(headingAllowance*headingAllowanceInBetween) &&
    currentHeadingY > headingNE_Y-(headingAllowance*headingAllowanceInBetween) && 
    currentHeadingY < headingNE_Y+(headingAllowance*headingAllowanceInBetween)
  ) return "NE";

  Serial.println("Direction is not identified");
  return "X";
}

void displayDirection(float currentHeadingX, float currentHeadingY) {
  Serial.print(currentHeadingX);
  Serial.print(", ");
  Serial.println(currentHeadingY);
  
  if(
    currentHeadingX >= headingN_X-headingAllowance && 
    currentHeadingX <= headingN_X+headingAllowance &&
    currentHeadingY >= headingN_Y-headingAllowance && 
    currentHeadingY <= headingN_Y+headingAllowance
  ) {
    Serial.print("\t\t");
    Serial.print(headingN_X);
    Serial.print(", ");
    Serial.print(headingN_Y);
    Serial.println(", N");

    digitalWrite(greenLed, HIGH);
    digitalWrite(yellowLed, LOW);
    digitalWrite(blueLed, LOW);
    digitalWrite(redLed, LOW);
  }
  else if(
    currentHeadingX > headingNW_X-(headingAllowance*headingAllowanceInBetween) && 
    currentHeadingX < headingNW_X+(headingAllowance*headingAllowanceInBetween) &&
    currentHeadingY > headingNW_Y-(headingAllowance*headingAllowanceInBetween) && 
    currentHeadingY < headingNW_Y+(headingAllowance*headingAllowanceInBetween)
  ) {
    Serial.print("\t\t");
    Serial.print(headingNW_X);
    Serial.print(", ");
    Serial.print(headingNW_Y);
    Serial.println("NW");

    digitalWrite(greenLed, HIGH);
    digitalWrite(yellowLed, HIGH);
    digitalWrite(blueLed, LOW);
    digitalWrite(redLed, LOW);
  }
  else if(
    currentHeadingX >= headingW_X-headingAllowance && 
    currentHeadingX <= headingW_X+headingAllowance &&
    currentHeadingY >= headingW_Y-headingAllowance && 
    currentHeadingY <= headingW_Y+headingAllowance
  ) {
    Serial.print("\t\t");
    Serial.print(headingW_X);
    Serial.print(", ");
    Serial.print(headingW_Y);
    Serial.println("W");

    digitalWrite(greenLed, LOW);
    digitalWrite(yellowLed, HIGH);
    digitalWrite(blueLed, LOW);
    digitalWrite(redLed, LOW);
  }
  else if(
    currentHeadingX > headingSW_X-(headingAllowance*headingAllowanceInBetween) && 
    currentHeadingX < headingSW_X+(headingAllowance*headingAllowanceInBetween) &&
    currentHeadingY > headingSW_Y-(headingAllowance*headingAllowanceInBetween) && 
    currentHeadingY < headingSW_Y+(headingAllowance*headingAllowanceInBetween)
  ) {
    Serial.print("\t\t");
    Serial.print(headingSW_X);
    Serial.print(", ");
    Serial.print(headingSW_Y);
    Serial.println("SW");

    digitalWrite(greenLed, LOW);
    digitalWrite(yellowLed, HIGH);
    digitalWrite(blueLed, HIGH);
    digitalWrite(redLed, LOW);
  }
  else if(
    currentHeadingX >= headingS_X-headingAllowance && 
    currentHeadingX <= headingS_X+headingAllowance &&
    currentHeadingY >= headingS_Y-headingAllowance && 
    currentHeadingY <= headingS_Y+headingAllowance
  ) {
    Serial.print("\t\t");
    Serial.print(headingS_X);
    Serial.print(", ");
    Serial.print(headingS_Y);
    Serial.println("S");

    digitalWrite(greenLed, LOW);
    digitalWrite(yellowLed, LOW);
    digitalWrite(blueLed, HIGH);
    digitalWrite(redLed, LOW);
  }
  else if(
    currentHeadingX > headingSE_X-(headingAllowance*headingAllowanceInBetween) && 
    currentHeadingX < headingSE_X+(headingAllowance*headingAllowanceInBetween) &&
    currentHeadingY > headingSE_Y-(headingAllowance*headingAllowanceInBetween) && 
    currentHeadingY < headingSE_Y+(headingAllowance*headingAllowanceInBetween)
  ) {
    Serial.print("\t\t");
    Serial.print(headingSE_X);
    Serial.print(", ");
    Serial.print(headingSE_Y);
    Serial.println("SE");

    digitalWrite(greenLed, LOW);
    digitalWrite(yellowLed, LOW);
    digitalWrite(blueLed, HIGH);
    digitalWrite(redLed, HIGH);
  }
  else if(
    currentHeadingX >= headingE_X-headingAllowance && 
    currentHeadingX <= headingE_X+headingAllowance &&
    currentHeadingY >= headingE_Y-headingAllowance && 
    currentHeadingY <= headingE_Y+headingAllowance
  ) {
    Serial.print("\t\t");
    Serial.print(headingE_X);
    Serial.print(", ");
    Serial.print(headingE_Y);
    Serial.println("E");

    digitalWrite(greenLed, LOW);
    digitalWrite(yellowLed, LOW);
    digitalWrite(blueLed, LOW);
    digitalWrite(redLed, HIGH);
  }
  else if(
    currentHeadingX > headingNE_X-(headingAllowance*headingAllowanceInBetween) && 
    currentHeadingX < headingNE_X+(headingAllowance*headingAllowanceInBetween) &&
    currentHeadingY > headingNE_Y-(headingAllowance*headingAllowanceInBetween) && 
    currentHeadingY < headingNE_Y+(headingAllowance*headingAllowanceInBetween)
  ) {
    Serial.print("\t\t");
    Serial.print(headingNE_X);
    Serial.print(", ");
    Serial.print(headingNE_Y);
    Serial.println("NE");

    digitalWrite(greenLed, HIGH);
    digitalWrite(yellowLed, LOW);
    digitalWrite(blueLed, LOW);
    digitalWrite(redLed, HIGH);
  }
}

void move(float currentHeadingX, float currentHeadingY) {
  float currentAngle = atan2(currentHeadingY, currentHeadingX);
  float savedAngle = atan2(savedHeadingY, savedHeadingX);
  float error = abs(currentAngle - savedAngle);
  float previous_error;
  float cumulative_error;
  int period = 50;

  float PID_p = kp * error;
  float PID_i = cumulative_error * ki;
  float PID_d = kd*(error - previous_error);

  double PID_total = PID_p + PID_i + PID_d;

  cumulative_error += error;
  previous_error = error;

  String currentDirection = getDirection(currentHeadingX, currentHeadingY);
  bool isStraight = false;
  bool isLeft = false;
  bool wasLeft = false;

  int modifiedSpeed = (int) PID_total;

  if(modifiedSpeed >= 20) {
    modifiedSpeed = 20;
  }

  if(testName == "N") {
    if(currentDirection == "N") {
      Serial.println("Straight");
      isStraight = true;
      isLeft = false;
    } else if(currentDirection == "NW" || currentDirection == "W" || currentDirection == "SW" || currentDirection == "S") {
      Serial.println("Left++");
      isStraight = false;
      isLeft = true;
    } else if(currentDirection == "NE" || currentDirection == "E" || currentDirection == "SE" || currentDirection == "S") {
      Serial.println("Right++");
      isStraight = false;
      isLeft = false;
    }
  } else if (testName == "NW") {
    if(currentDirection == "NW") {
      Serial.println("Straight");
      isStraight = true;
      isLeft = false;
    } else if(currentDirection == "W" || currentDirection == "SW" || currentDirection == "S" || currentDirection == "SE") {
      Serial.println("Left++");
      isStraight = false;
      isLeft = true;
    } else if(currentDirection == "N" || currentDirection == "NE" || currentDirection == "E" || currentDirection == "SE") {
      Serial.println("Right++");
      isStraight = false;
      isLeft = false;
    }
  } else if (testName == "W") {
    if(currentDirection == "W") {
      Serial.println("Straight");
      isStraight = true;
      isLeft = false;
    } else if(currentDirection == "SW" || currentDirection == "S" || currentDirection == "SE" || currentDirection == "E") {
      Serial.println("Left++");
      isStraight = false;
      isLeft = true;
    } else if(currentDirection == "NW" || currentDirection == "N" || currentDirection == "NE" || currentDirection == "E") {
      Serial.println("Right++");
      isStraight = false;
      isLeft = false;
    }
  } else if (testName == "SW") {
    if(currentDirection == "SW") {
      Serial.println("Straight");
      isStraight = true;
      isLeft = false;
    } else if(currentDirection == "S" || currentDirection == "SE" || currentDirection == "E" || currentDirection == "NE") {
      Serial.println("Left++");
      isStraight = false;
      isLeft = true;
    } else if(currentDirection == "W" || currentDirection == "NW" || currentDirection == "N" || currentDirection == "E") {
      Serial.println("Right++");
      isStraight = false;
      isLeft = false;
    }
  } else if (testName == "S") {
    if(currentDirection == "S") {
      Serial.println("Straight");
      isStraight = true;
      isLeft = false;
    } else if(currentDirection == "SE" || currentDirection == "E" || currentDirection == "NE" || currentDirection == "N") {
      Serial.println("Left++");
      isStraight = false;
      isLeft = true;
    } else if(currentDirection == "SW" || currentDirection == "W" || currentDirection == "NW" || currentDirection == "N") {
      Serial.println("Right++");
      isStraight = false;
      isLeft = false;
    }
  } else if (testName == "SE") {
    if( currentDirection == "SE") {
      Serial.println("Straight");
      isStraight = true;
      isLeft = false;
    } else if(currentDirection == "E" ||currentDirection == "NE" || currentDirection == "N" || currentDirection == "NW") {
      Serial.println("Left++");
      isStraight = false;
      isLeft = true;
    } else if(currentDirection == "S" || currentDirection == "SW" || currentDirection == "W" || currentDirection == "NW") {
      Serial.println("Right++");
      isStraight = false;
      isLeft = false;
    }
  } else if (testName == "E") {
    if(currentDirection == "E") {
      Serial.println("Straight");
      isStraight = true;
      isLeft = false;
    } else if(currentDirection == "NE" ||currentDirection == "N" || currentDirection == "NW" || currentDirection == "NE") {
      Serial.println("Left++");
      isStraight = false;
      isLeft = true;
    } else if(currentDirection == "SE" || currentDirection == "S" || currentDirection == "SW" || currentDirection == "W") {
      Serial.println("Right++");
      isStraight = false;
      isLeft = false;
    }
  } else if (testName == "NE") {
    if(currentDirection == "NE") {
      Serial.println("Straight");
      isStraight = true;
      isLeft = false;
    } else if(currentDirection == "N" || currentDirection == "NW" || currentDirection == "W" || currentDirection == "SW") {
      Serial.println("Left++");
      isStraight = false;
      isLeft = true;
    } else if(currentDirection == "E" || currentDirection == "SE" || currentDirection == "S" || currentDirection == "SW") {
      Serial.println("Right++");
      isStraight = false;
      isLeft = false;
    }
  }

  if(isStraight && !isLeft) {
    digitalWrite(greenLed, LOW);
    digitalWrite(yellowLed, HIGH);
    digitalWrite(blueLed, HIGH);
    digitalWrite(redLed, LOW);
    if(wasLeft) {
      escLeft.write(modifiedSpeed+15);
      escRight.write(15);
    } else {
      escLeft.write(15);
      escRight.write(modifiedSpeed);
    }
  } else if(!isStraight && !isLeft) {
    digitalWrite(greenLed, HIGH);
    digitalWrite(yellowLed, LOW);
    digitalWrite(blueLed, LOW);
    digitalWrite(redLed, LOW);
    escLeft.write(15);
    escRight.write(modifiedSpeed);
    wasLeft = false;
  } else if(!isStraight && isLeft) {
    digitalWrite(greenLed, LOW);
    digitalWrite(yellowLed, LOW);
    digitalWrite(blueLed, LOW);
    digitalWrite(redLed, HIGH);
    escLeft.write(modifiedSpeed+15);
    escRight.write(15);
    wasLeft = true;
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
    Nano.println(sentMessage);
  } else {
    Serial.println("Wrong format of command. Try again.");
  }
}

bool receivedSpecificCommand(String command) {
  return receiveCommand() && (receivedCommand == command);
}