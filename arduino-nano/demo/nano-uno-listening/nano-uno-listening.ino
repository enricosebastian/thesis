#include <NeoSWSerial.h>

String message = "";

const int txNano = 12;
const int rxNano = 11;

NeoSWSerial Nano(rxNano, txNano);

void setup() {
  Serial.begin(9600);
  Nano.begin(9600);

  Nano.listen();
  Serial.println("Nano initializing...");
}

void loop() {

  if(Nano.available()) {
    char letter = Nano.read();
    Serial.println(letter);
  }

}