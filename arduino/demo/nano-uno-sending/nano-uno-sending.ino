#include <NeoSWSerial.h>

String message = "";

const int txNano = A2;
const int rxNano = A3;

NeoSWSerial Nano(rxNano, txNano);

void setup() {
  Serial.begin(9600);
  Nano.begin(9600);

  Nano.listen();
  Serial.println("Nano initializing...");
}

void loop() {
  Serial.println("Sending: Hello");
  Nano.println("Hallo\n");
  delay(800);

}