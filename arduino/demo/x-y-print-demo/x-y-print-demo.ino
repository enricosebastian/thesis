#include <NeoSWSerial.h>

const int rxPin = A4;
const int txPin = A5;

NeoSWSerial ser(rxPin, txPin);

String message = "";

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
      message += '\n';
      Serial.println(message);
      message = "";
    } else {
      message += letter;
    }
  }
}
