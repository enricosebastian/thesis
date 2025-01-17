#include <SPI.h>
#include "DW1000Ranging.h"
#include "DW1000.h"

#define TAG_ADD "01:20:5B:D5:A9:9A:E2:9C"

#define TX 16
#define RX 17

#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23

const uint8_t PIN_RST = 27;
const uint8_t PIN_IRQ = 34;
const uint8_t PIN_SS = 4;

String message = "";
String details = "";

float r1 = 0.0;
float r2 = 0.0;
float currentR1 = 0.0;
float currentR2 = 0.0;
float pastR1 = currentR1;
float pastR2 = currentR2;

float x0 = 3.7;
float xval1, yval1;
char xc1[10], yc1[10];
int ctr;

uint32_t runtime;
uint32_t push; // time to push data to arduino

float startTime = 0;
float startTime2 = 0;
float waitingTime = 300;

bool isLookingForX = true;

int correctTimes = 0;
int maxCorrectTimes = 30;

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600,SERIAL_8N1,RX,TX);
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ);
  DW1000Ranging.attachNewRange(0);
  DW1000Ranging.attachNewDevice(0);
  DW1000Ranging.attachInactiveDevice(0);

  DW1000Ranging.startAsTag(TAG_ADD,DW1000.MODE_LONGDATA_FAST_ACCURACY,DW1000.CHANNEL_5,false);

  // DW1000Ranging.startAsTag(TAG_ADD,DW1000.MODE_LONGDATA_RANGE_LOWPOWER,DW1000.CHANNEL_5,false);

  startTime = millis();
  startTime2 = millis();
}

void loop() {
  DW1000Ranging.loop();

  // For Anchor 1
  if(DW1000Ranging.getDistantDevice()->getShortAddress() == 0x1001 && isLookingForX) {

    if(DW1000Ranging.getDistantDevice()->getRange() != 0) {
      r1 = DW1000Ranging.getDistantDevice()->getRange();     
      correctTimes++; // Error-correction implementation
    }

    // If you are completely sure that the last coordinate from Anchor 1 is correct, switch channels
    if(correctTimes > maxCorrectTimes) {
      Serial.println("Switching to channel 7");
      DW1000Ranging.startAsTag(TAG_ADD,DW1000.MODE_LONGDATA_FAST_ACCURACY,DW1000.CHANNEL_7,false);
      correctTimes = 0;
      isLookingForX = false;
    }

  // For anchor 2
  } else if(DW1000Ranging.getDistantDevice()->getShortAddress() == 0x1002 && !isLookingForX) {
    if(DW1000Ranging.getDistantDevice()->getRange() != 0) {
      r2 = DW1000Ranging.getDistantDevice()->getRange();
      correctTimes++; // Error-correction implementation
    }

    // If you are completely sure that the last coordinate from Anchor 2 is correct, switch channels
    if(correctTimes > maxCorrectTimes) {
      Serial.println("Switching to channel 5");
      DW1000Ranging.startAsTag(TAG_ADD,DW1000.MODE_LONGDATA_FAST_ACCURACY,DW1000.CHANNEL_5,false);
      correctTimes = 0;
      isLookingForX = true;
    }
  }

  Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress());
  Serial.print(" - ");
  Serial.print(r1);
  Serial.print(",");
  Serial.println(r2);

  if(millis() - startTime2 > 800) {
    message = "COOR ALL ALL " + String(r1) + "," + String(r2);
    Serial2.println(message);
    startTime2 = millis();
  }

}