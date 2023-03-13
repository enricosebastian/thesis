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
  message = "CONN DRO1 YEET HI";

  HC12.println(message);
  Serial.println(message);
  delay(500);
}