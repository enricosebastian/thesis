#include <NeoSWSerial.h>

const int txUno = A2; //green tx
const int rxUno = A3; //blue rx

String message =  "";

NeoSWSerial Uno(txUno, rxUno); // (Green TX, Blue RX)

void setup() {
  Serial.begin(9600);
  Uno.begin(9600);
}

void loop() {
  if(Serial.available()) {
    char letter = Serial.read();
    if(letter == '\n') {
      message += '\n';
      Serial.println(message);
      Uno.println(message);
      message = "";
    } else {
      message += letter;
    }
  }
}