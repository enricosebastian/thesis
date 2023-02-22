#include <NeoSWSerial.h>

const int rxPin = A4;
const int txPin = A5;

const float x0 = 9.5;

float r1 = 0;
float r2 = 0;

float posX = 0;
float posY = 0;

NeoSWSerial ser(rxPin, txPin);

String receivedMessage = "";
String receivedCommand = "";
String receivedToName = "";
String receivedFromName = "";
String receivedDetails = "";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  ser.begin(9600);
  ser.listen();

  Serial.println("Coordinates demo initializing");
}

void loop() {
  // put your main code here, to run repeatedly:
  if(ser.available()) {
    char letter = ser.read();
    if(letter == '\n') {
      receivedMessage += '\n';

      int endIndex = receivedMessage.indexOf(' ');
      receivedCommand = receivedMessage.substring(0, endIndex);
      receivedMessage = receivedMessage.substring(endIndex+1);

      endIndex = receivedMessage.indexOf(' ');
      receivedToName = receivedMessage.substring(0, endIndex);
      receivedMessage = receivedMessage.substring(endIndex+1);

      endIndex = receivedMessage.indexOf(' ');
      receivedFromName = receivedMessage.substring(0, endIndex);
      receivedDetails = receivedMessage.substring(endIndex+1);

      endIndex = receivedDetails.indexOf(',');
      r1 = receivedDetails.substring(0, endIndex).toFloat();
      r2 = receivedDetails.substring(endIndex+1).toFloat();

      posX = (x0*x0 - r2*r2 + r1*r1)/(2*x0);
      posY = sqrt(r1*r1 - posX*posX);

      Serial.print(posX);
      Serial.print(",");
      Serial.println(posY);

      receivedMessage = "";
    } else {
      receivedMessage += letter;
    }
  }
}
