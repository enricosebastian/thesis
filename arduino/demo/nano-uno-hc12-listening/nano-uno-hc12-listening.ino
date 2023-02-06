#include <NeoSWSerial.h>

//Constants (buttons)
const int txHc12 = A0; //green tx
const int rxHc12 = A1; //blue received
const int txNano = 9; //green tx
const int rxNano = 8; //blue received
const int waitingTime = 5000;

bool isListeningToHc12 = true;
bool isListeningToNano = false;

String message = "";

NeoSWSerial HC12(txHc12, rxHc12); // (Green TX, Blue RX)
NeoSWSerial Nano(txNano, rxNano); // (Green TX, Blue RX)

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);
  Nano.begin(9600);
  HC12.listen();
  Serial.println("Initializing...");
}

void loop() {
  if(!isListeningToNano && HC12.available()) {
    char letter = HC12.read();
    Serial.println(letter);
  }

  if(Serial.available()) {
    char letter = Serial.read();
    if(letter == "\n") {
      message += "\n";
      isListeningToNano = true;
    } else {
      message += letter;
    }
  }

  if(isListeningToNano) {
    HC12.end();
    Nano.listen();
    Nano.print(message);
    Nano.end();
    isListeningToNano = false;
    HC12.listen();
  }
  
}