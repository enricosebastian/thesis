#include <NeoSWSerial.h>

const int rxNano = 11; 
const int txNano = 12; 

const int rxEsp = 10; 
const int txEsp = 9; 

bool isNano = true;

NeoSWSerial Nano(rxNano, txNano);
NeoSWSerial Esp(rxEsp, txEsp);

void setup() {
  Serial.begin(9600);
  Nano.begin(9600);
  Esp.begin(9600);

  Nano.listen();

  Serial.println("Nano initializing...");
}

void loop() {

  if(Nano.available()) {
    char letter = Nano.read();
    Serial.print("Nano - ");
    Serial.println(letter);
    if(letter == '\n') {
      if(isNano) {
        Nano.end();
        Esp.listen();
      } else {
        Esp.end();
        Nano.listen();
      }
      isNano = !isNano;
    }
  }
}