#include <NeoSWSerial.h>

const int rxNano = 11; 
const int txNano = 12; 
const int rxEsp = 10; 
const int txEsp = 9; 

NeoSWSerial Nano(rxNano, txNano);
NeoSWSerial Esp(rxEsp, txEsp);

void setup() {
  Serial.begin(9600);
  Nano.begin(9600);
  Esp.begin(9600);

  Serial.println("Arduino initializing...");
}

void loop() {
  Nano.listen();
  Esp.end(); 

  String message = "HelloNano";
  Serial.print("Sending: ");
  Serial.println(message);
  Nano.println(message);

  delay(500);
  Nano.end();
  Esp.listen(); 

  message = "HelloEsp";
  Serial.print("Sending: ");
  Serial.println(message);
  Esp.println(message);

  delay(500);
}