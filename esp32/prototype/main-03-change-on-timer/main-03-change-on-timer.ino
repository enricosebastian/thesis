#include <SPI.h>
#include "DW1000Ranging.h"
#include "DW1000.h"

// #define TAG_ADD "01:20:5B:D5:A9:9A:E2:9C" // Tag 1
#define TAG_ADD "02:20:5B:D5:A9:9A:E2:9C" // Tag 2
// #define TAG_ADD "03:20:5B:D5:A9:9A:E2:9C" // Tag 3

#define TX 16
#define RX 17

#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23

const uint8_t PIN_RST = 27;
const uint8_t PIN_IRQ = 34;
const uint8_t PIN_SS = 4;

String message = "";

String tempChannel = "";

// // tag 1
// String channel1 = "5";
// String channel2 = "7";
// String channel3 = "9";

// tag 2
String channel1 = "7";
String channel2 = "9";
String channel3 = "5";

// // tag 3
// String channel1 = "9";
// String channel2 = "5";
// String channel3 = "7";

float r1 = 0.0;
float r2 = 0.0;

float startTime = 0;
float startTime2 = 0;
float waitingTime = 300;

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600,SERIAL_8N1,RX,TX);
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ);
  DW1000Ranging.attachNewRange(0);
  DW1000Ranging.attachNewDevice(0);
  DW1000Ranging.attachInactiveDevice(0);

  DW1000Ranging.startAsTag(TAG_ADD,DW1000.MODE_LONGDATA_RANGE_LOWPOWER,DW1000.CHANNEL_7,false);

  startTime = millis();
}

void loop() {
  DW1000Ranging.loop();

  if(millis() - startTime > 2000) {
    tempChannel = channel1;
    channel1 = channel2;
    channel2 = channel3;
    channel3 = tempChannel;

    if(channel1 == "5") {
      DW1000Ranging.startAsTag(TAG_ADD,DW1000.MODE_LONGDATA_RANGE_LOWPOWER,DW1000.CHANNEL_5,false);
    } else if(channel1 == "7") {
      DW1000Ranging.startAsTag(TAG_ADD,DW1000.MODE_LONGDATA_RANGE_LOWPOWER,DW1000.CHANNEL_7,false);
    } else if(channel1 == "9") {

    }

    startTime = millis();
  }

  // For Anchor 1
  if(DW1000Ranging.getDistantDevice()->getShortAddress() == 0x1001 && channel1 == "5") {

    if(DW1000Ranging.getDistantDevice()->getRange() != 0) {
      r1 = DW1000Ranging.getDistantDevice()->getRange();
    }

  // For anchor 2
  } else if(DW1000Ranging.getDistantDevice()->getShortAddress() == 0x1002 && channel1 == "7") {
    
    if(DW1000Ranging.getDistantDevice()->getRange() != 0) {
      r2 = DW1000Ranging.getDistantDevice()->getRange();
    }

  } else if(channel1 == "9") {
    Serial.println("Doing nothing");    
  }

  Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress());
  Serial.print(" - ");
  Serial.print(r1);
  Serial.print(",");
  Serial.println(r2);

  if(millis() - startTime2 > 800) {
    message = "COOR ALL ALL " + String(r1) + "," + String(r2);
    Serial2.println(message);
    message = "";
    startTime2 = millis();
  }
  
}