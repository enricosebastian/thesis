String message;

void setup() {
  Serial.begin(9600);
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);
}

void loop() {
  
  if(Serial.available()) {
    message = Serial.readStringUntil('\n');
    message.trim();
    if(message == "hello") {
      digitalWrite(8,HIGH);
    } else if(message == "goodbye") {
      digitalWrite(9,HIGH);
    } else {
      digitalWrite(8,LOW);
      digitalWrite(9,LOW);
    }
  }

  
}
