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

float r1 = 6969.6969;
float r2 = 6767.6767;
float currentR1 = 6969.6969;
float currentR2 = 6969.6969;
float pastR1 = currentR1;
float pastR2 = currentR2;

float x0 = 3.7;
float xval1, yval1;
char xc1[10], yc1[10];
int ctr;

uint32_t runtime;
uint32_t push; // time to push data to arduino

float startTime = 0;
float waitingTime = 300;

bool isLookingForX = true;

int correctTimes = 0;
int maxCorrectTimes = 50;

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600,SERIAL_8N1,RX,TX);
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ);
  DW1000Ranging.attachNewRange(0);
  DW1000Ranging.attachNewDevice(0);
  DW1000Ranging.attachInactiveDevice(0);

  DW1000Ranging.startAsTag(TAG_ADD,DW1000.MODE_LONGDATA_RANGE_LOWPOWER,DW1000.CHANNEL_5,false);

  startTime = millis();
}

void loop() {
  DW1000Ranging.loop();

  if(DW1000Ranging.getDistantDevice()->getShortAddress() == 0x1001) {
    if(DW1000Ranging.getDistantDevice()->getRange() != 0) {
      r1 = DW1000Ranging.getDistantDevice()->getRange();     
      correctTimes++; 
    }

    if(correctTimes > maxCorrectTimes) {
      DW1000Ranging.startAsTag(TAG_ADD,DW1000.MODE_LONGDATA_RANGE_LOWPOWER,DW1000.CHANNEL_7,false);
      correctTimes = 0;
    }
  } else if(DW1000Ranging.getDistantDevice()->getShortAddress() == 0x1002) {
    if(DW1000Ranging.getDistantDevice()->getRange() != 0) {
      r2 = DW1000Ranging.getDistantDevice()->getRange();
      correctTimes++;
    }

    if(correctTimes > maxCorrectTimes) {
      DW1000Ranging.startAsTag(TAG_ADD,DW1000.MODE_LONGDATA_RANGE_LOWPOWER,DW1000.CHANNEL_5,false);
      correctTimes = 0;
    }
  }

  message = String(r1) + "," + String(r2);
  Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress());
  Serial.print(" - ");
  Serial.println(message);
  Serial2.println(message);
}