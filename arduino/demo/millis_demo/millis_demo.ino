unsigned long myTime;
unsigned long myEndTime;

void setup() {
  Serial.begin(9600);
}
void loop() {
  Serial.print("Time: ");
  myEndTime = millis() + 60000;
  
  while(millis() <= myEndTime) {
    Serial.print("Time: ");
    Serial.println(millis());
  }
  Serial.println("End");
  while(true) {
    //do nothing
  }
}
