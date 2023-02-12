#include <NeoSWSerial.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

//Constants (buttons)
const int txHc12 = 2; //green tx
const int rxHc12 = 3; //blue received

String message = "";

NeoSWSerial HC12(txHc12, rxHc12); // (Green TX, Blue RX)

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);
  HC12.listen();
  Serial.println("Initializing nano...");
}

void loop() {

  if(HC12.available()) {
    char letter = HC12.read();
    if(letter == '\n') {
      message += '\n';
      Serial.println(message);
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
      Serial.print(message);
      HC12.print(message);
      message = "";
    } else {
      message += letter;
    }
  }



}