#include <NeoSWSerial.h>

//Constants (buttons)
const int txHc12 = A0; //green tx
const int rxHc12 = A1; //blue received

String message = "";

float heading = 0.00;
float pastHeading = 0.00;

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
      heading = message.toFloat();

      if(heading != pastHeading) {
        if(heading-pastHeading < 0) {
          Serial.println("turned \tleft");
        } else if(heading - pastHeading > 0) {
          Serial.println("turned \t\tright");
        }
        pastHeading = heading;
      } else {
        Serial.println("center");
      }
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