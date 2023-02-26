#include <SPI.h>
#include "DW1000Ranging.h"
#include "DW1000.h"

#define TAG_ADD "01:20:5B:D5:A9:9A:E2:9C" // Tag 1
//#define TAG_ADD "02:20:5B:D5:A9:9A:E2:9C" // Tag 2

#define TX 16
#define RX 17

#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23

const uint8_t PIN_RST = 27;
const uint8_t PIN_IRQ = 34;
const uint8_t PIN_SS = 4;

String message = "";

float r1 = 0.0;
float r2 = 0.0;

float startTime = 0;
float waitingTime = 300;

bool isLookingForX = false;
bool isLookingForY = false;

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

  startTime = millis();
}

void loop() {
  DW1000Ranging.loop();

  if(Serial2.available()) {
    char letter = Serial.read();
    Serial.println(letter);
    if(letter == '\n') {
      Serial.print("rec: ");
      Serial.println(message);
      if(message == "CHAN5") {
        Serial.println("Changing to Channel 5");
        DW1000Ranging.startAsTag(TAG_ADD,DW1000.MODE_LONGDATA_RANGE_LOWPOWER,DW1000.CHANNEL_5,false);
      } else if (message == "CHAN7") {
        Serial.println("Changing to Channel 7");
        DW1000Ranging.startAsTag(TAG_ADD,DW1000.MODE_LONGDATA_RANGE_LOWPOWER,DW1000.CHANNEL_7,false);
      } else if (message == "CHAN9") {
        Serial.println("Changing to Channel 9");
      }
    } else {
      message += letter;
    }
  }

  // For Anchor 1
  if(DW1000Ranging.getDistantDevice()->getShortAddress() == 0x1001) {

    if(DW1000Ranging.getDistantDevice()->getRange() != 0) {
      r1 = DW1000Ranging.getDistantDevice()->getRange();
    }

  // For anchor 2
  } else if(DW1000Ranging.getDistantDevice()->getShortAddress() == 0x1002 && !isLookingForX) {
    if(DW1000Ranging.getDistantDevice()->getRange() != 0) {
      r2 = DW1000Ranging.getDistantDevice()->getRange();
    }
  }

  Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress());
  Serial.print(" - ");
  Serial.print(r1);
  Serial.print(",");
  Serial.println(r2);

  if(millis() - startTime > 800) {
    message = "COOR ALL ALL " + String(r1) + "," + String(r2);
    Serial2.println(message);
    startTime = millis();
  }
  
}