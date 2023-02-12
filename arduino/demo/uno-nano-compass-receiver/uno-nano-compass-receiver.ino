#include <NeoSWSerial.h>

//Constants (buttons)
const int txHc12 = A0; //green tx
const int rxHc12 = A1; //blue received

String message = "";

NeoSWSerial HC12(txHc12, rxHc12); // (Green TX, Blue RX)

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);
  HC12.listen();
  Serial.println("Initializing...");
}

void loop() {

  if(HC12.available()) {
    char letter = HC12.read();
    if(letter == '\n') {
      message += '\n';
      Serial.print(message);
      message = "";
    } else {
      message += letter;
    }
  }

  if(Serial.available()) {
    char letter = Serial.read();
    if(letter == '\n') {
      message += '\n';
      Serial.print("Sending: ");
      Serial.println(message);
      HC12.print(message);
    } else {
      message += letter;
    }
  }



}