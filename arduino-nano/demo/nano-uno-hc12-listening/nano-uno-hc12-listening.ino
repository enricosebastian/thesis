#include <NeoSWSerial.h>

const int redLed = 2;
String message = "";

const int txNano = 9;
const int rxNano = 8;

NeoSWSerial Nano(txNano, rxNano);

void setup() {
  Serial.begin(9600);
  Nano.begin(9600);

  Nano.listen();
  pinMode(redLed, OUTPUT);
  digitalWrite(redLed, LOW);
  Serial.println("Nano initializing...");
}

void loop() {

  if(Nano.available()) {
    char letter = Nano.read();
    if(letter == '\n') {
      message += '\n';
      Serial.println(message);
      if(message == "OFF\n") {
        digitalWrite(redLed, LOW);
      } else if(message == "ON\n") {
        digitalWrite(redLed, HIGH);
      }
      message = "";
    } else {
      message += letter;
    }
  }

}