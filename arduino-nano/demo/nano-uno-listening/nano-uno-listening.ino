#include <NeoSWSerial.h>

String message = "";

const int rxNano = 11; 
const int txNano = 12; 

const int yellowLed = 9;
const int redLed = 10;

NeoSWSerial Nano(rxNano, txNano);

void setup() {
  Serial.begin(9600);
  Nano.begin(9600);

  Nano.listen();
  Serial.println("Nano initializing...");

  pinMode(redLed, OUTPUT);
  digitalWrite(redLed, HIGH);
}

void loop() {

  if(Nano.available()) {
    digitalWrite(redLed, HIGH);
    char letter = Nano.read();
    Serial.println(letter);
  } else {
    digitalWrite(redLed, LOW);
  }

}