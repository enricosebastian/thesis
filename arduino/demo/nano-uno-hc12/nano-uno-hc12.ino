#include <NeoSWSerial.h>

//Constants (buttons)
const int rxHc12 = A0; //green wire
const int txHc12 = A1; //blue wire
const int rxNano = A2; //green wire
const int txNano = A3; //blue wire
const int rxEsp = A4;
const int txEsp = A5;

String message = "";

// SoftwareSerial(rxPin, txPin, inverse_logic)
NeoSWSerial HC12(rxHc12, txHc12);
NeoSWSerial Nano(rxNano, txNano);

void setup() {
  Serial.begin(9600);
  Nano.begin(9600);

  Nano.listen();
  Serial.println("Initializing...");
}

void loop() {

  Nano.println("Hallo");
  Serial.println("Hallo");
  delay(500);
  
}