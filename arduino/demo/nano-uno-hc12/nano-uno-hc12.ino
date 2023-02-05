#include <NeoSWSerial.h>

//Constants (buttons)
const int txHc12 = A0; //green tx
const int rxHc12 = A1; //blue received
const int txNano = 9; //green tx
const int rxNano = 8; //blue received
const int waitingTime = 5000;

String message = "";

NeoSWSerial HC12(txHc12, rxHc12); // (Green TX, Blue RX)
NeoSWSerial Nano(txNano, rxNano); // (Green TX, Blue RX)

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);
  Nano.begin(9600);
  Serial.println("Initializing...");
}

void loop() {

  HC12.listen();
  while(HC12.available()) {
    Serial.println("hc12 listen...");
  }
  HC12.end();

  while(Serial.available()) {
    char letter = Serial.read();
    if(letter == '\n') {
      message += '\n';

      Serial.print("Sending: ");
      Serial.println(message);
      HC12.listen();
      HC12.println(message);
      HC12.end();
      message = "";
    } else {
      message += letter;
    }
  }
}