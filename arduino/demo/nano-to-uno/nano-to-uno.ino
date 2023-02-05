const int redLed = 4;

String message = "";

void setup() {
  pinMode(redLed, OUTPUT);
  digitalWrite(redLed, LOW);

  Serial.begin(9600);
}

void loop() {
  if(Serial.available()) {
    char letter = Serial.read();
    if(letter == '\n') {
      message += '\n';
      if(message == "ON\n") {
        digitalWrite(redLed, HIGH);
      } else if(message == "OFF\n") {
        digitalWrite(redLed, LOW);
      }
      message = "";
    } else {
      message += letter;
    }
  }
}