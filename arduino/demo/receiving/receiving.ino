#include <NeoSWSerial.h>

//Constants (buttons)
const int rxHc12 = A0; //green wire
const int txHc12 = A1; //blue wire

String message = "";

// SoftwareSerial(rxPin, txPin, inverse_logic)
NeoSWSerial HC12(rxHc12, txHc12);

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);
  Serial.println("Initializing...");
}

void loop() {

  while(HC12.available()) {
    char letter = HC12.read();
    if(letter == '\n') {
      Serial.println(message);
      message = "";
    } else {
      message += letter;
    }
  }
  
}