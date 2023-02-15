void setup(void) 
{
  Serial.begin(9600);


  float savedAngle = 270+45;
  float oppositeSavedAngle = savedAngle + 180;
  if(oppositeSavedAngle > 360) {
    oppositeSavedAngle = oppositeSavedAngle - 360;
  }

  float iterationsDone = 0;
  for(int i = 0; i <= 360; i++) {
    float currentAngle = i;
    float oppositeCurrentAngle = 360-currentAngle;

    bool isLeft = ((currentAngle > savedAngle) && (currentAngle <= oppositeSavedAngle)) || 
                  ((oppositeSavedAngle < savedAngle) && (currentAngle > savedAngle) && (currentAngle <= 360)) ||
                  ((oppositeSavedAngle < savedAngle) && (currentAngle < oppositeSavedAngle))
                  ;

    Serial.print("Save: ");
    Serial.print(savedAngle);

    Serial.print("Curr: ");
    Serial.print(currentAngle);
    

    if(isLeft) {
      Serial.println("left");
    } else {
      Serial.println("right");
    }
    Serial.println();
  }

  Serial.print("Done!");

}

void loop() {

}



// if(isLeft) {
//   Serial.print("Save: ");
//   Serial.print(savedAngle);
//   Serial.print(", ");
//   Serial.println(oppositeSavedAngle);

//   Serial.print("Curr: ");
//   Serial.print(currentAngle);
//   Serial.print(", ");
//   Serial.println(oppositeCurrentAngle);
//   Serial.println("left");
//   Serial.println();

//   iterationsDone++;
// }