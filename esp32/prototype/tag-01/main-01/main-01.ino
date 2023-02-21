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

  if(millis() - startTime > 2000) {
    if(isLookingForX) {
      DW1000Ranging.startAsTag(TAG_ADD,DW1000.MODE_LONGDATA_RANGE_LOWPOWER,DW1000.CHANNEL_5,false);
      isLookingForX = false;
    } else {
      DW1000Ranging.startAsTag(TAG_ADD,DW1000.MODE_LONGDATA_RANGE_LOWPOWER,DW1000.CHANNEL_7,false);
      isLookingForX = true;
    }
    startTime = millis();
  }

  if(DW1000Ranging.getDistantDevice()->getShortAddress() == 0x1001) {
    if(r1 != 0) r1 = DW1000Ranging.getDistantDevice()->getRange();
  } else if(DW1000Ranging.getDistantDevice()->getShortAddress() == 0x1002) {
    if(r2 != 0) r2 = DW1000Ranging.getDistantDevice()->getRange();
  }

  Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress());
  Serial.print(" - ");
  Serial.print(r1);
  Serial.print(",");
  Serial.println(r2);
}

void coord(){
  xval1 = (x0*x0 - r2*r2 + r1*r1)/(2*x0);
  yval1 = sqrt(r1*r1 - xval1*xval1);
  if(xval1 < 0) xval1 = 0;
  else if(isnan(xval1)) xval1 = 0;
  if(isnan(yval1)) yval1 = 0;
  dtostrf(xval1,3,2,xc1);
  dtostrf(yval1,3,2,yc1);
}
