void setup(void) 
{
  Serial.begin(9600);


  float savedAngle = 360;
  
  float oppositeSavedAngle = savedAngle + 180;
  if(oppositeSavedAngle > 360) {
    oppositeSavedAngle = oppositeSavedAngle - 360;
  }

  for(int i = 0; i <= 360; i++) {
    float currentAngle = i;
    float oppositeCurrentAngle = 360-currentAngle;

    bool isLeft = ((currentAngle > savedAngle) && (currentAngle <= oppositeSavedAngle)) || 
                  ((oppositeSavedAngle < savedAngle) && (currentAngle > savedAngle) && (currentAngle <= 360)) ||
                  ((oppositeSavedAngle < savedAngle) && (currentAngle < oppositeSavedAngle))
                  ;

    Serial.print("Save: ");
    Serial.println(savedAngle);

    Serial.print("Curr: ");
    Serial.println(currentAngle);

    if(isLeft) {
      Serial.println("\tleft");
    } else {
      Serial.println("\t\tright");
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