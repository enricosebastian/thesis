#include <SoftwareSerial.h>

const int txPin = A0;
const int rxPin = A1;
const int ledPin = 13;

String message = "";

SoftwareSerial ss(rxPin, txPin);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  ss.begin(9600);

  pinMode(ledPin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(ss.available()) {
    char letter = ss.read();
    if(letter == '\n') {
      message += '\n';
      Serial.println(message);
      message = "";
    } else {
      message += letter;
    }
    digitalWrite(ledPin, LOW);
  } else {
    digitalWrite(ledPin, LOW);
  }
}
