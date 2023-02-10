#include <Wire.h>
#include <HMC5883L_Simple.h>

HMC5883L_Simple Compass;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Wire.begin();
  Compass.SetDeclination(-2, 37, 'W');  
  
  Serial.println("Initializing");

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(Compass.GetHeadingDegrees());
  delay(500);
}
