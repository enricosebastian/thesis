#include <SPI.h>
#include "DW1000Ranging.h"
#include "DW1000.h"

#define TAG_ADD "03:20:5B:D5:A9:9A:E2:9C" // Tag 3

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

// tag 2
String channel1 = "1";
String channel2 = "1";
String channel3 = "1";
String channel4 = "1";
String channel5 = "1";
String channel6 = "1";
String channel7 = "1";
String channel8 = "1";
String channel9 = "5";
String channel10 = "1";
String channel11 = "7";
String channel12 = "1";

float t1 = 0.0;
float t2 = 0.0;
float r1 = 0.0;
float r2 = 0.0;

unsigned long startTime = 0;
unsigned long startTime2 = 0;
unsigned long waitingTime = 300;

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600,SERIAL_8N1,RX,TX);
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ);
  DW1000Ranging.attachNewRange(0);
  DW1000Ranging.attachNewDevice(0);
  DW1000Ranging.attachInactiveDevice(0);

  Serial.println("Starting Tag 3");

  DW1000Ranging.startAsTag(TAG_ADD,DW1000.MODE_LONGDATA_RANGE_LOWPOWER,DW1000.CHANNEL_7,false);
}

void loop() {
  DW1000Ranging.loop();

  if(channel1 != "1") {
    // For Anchor 1
    if(DW1000Ranging.getDistantDevice()->getShortAddress() == 0x1001 && channel1 == "5") {

      t1 = DW1000Ranging.getDistantDevice()->getRange();
      if(t1 > 0 && t1 < 30) {
        r1 = t1;
      }

      Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress());
      Serial.print(" - ");
      Serial.print(r1);
      Serial.print(",");
      Serial.println(r2);
    // For anchor 2
    } else if(DW1000Ranging.getDistantDevice()->getShortAddress() == 0x1002 && channel1 == "7") {
      
      t2 = DW1000Ranging.getDistantDevice()->getRange();
      if(t2 > 0 && t2 < 30) {
        r2 = t2;
      }

      Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress());
      Serial.print(" - ");
      Serial.print(r1);
      Serial.print(",");
      Serial.println(r2);
    }
  }
  
  if(millis() - startTime > 1000) {
    tempChannel = channel1;
    channel1 = channel2;
    channel2 = channel3;
    channel3 = channel4;
    channel4 = channel5;
    channel5 = channel6;
    channel6 = channel7;
    channel7 = channel8;
    channel8 = channel9;
    channel9 = channel10;
    channel10 = channel11;
    channel11 = channel12;
    channel12 = tempChannel;

    Serial.print("Changing to channel ");
    Serial.println(channel1);

    if(channel1 == "5") {
      DW1000Ranging.startAsTag(TAG_ADD,DW1000.MODE_LONGDATA_RANGE_LOWPOWER,DW1000.CHANNEL_5,false);
    } else if(channel1 == "7") {
      DW1000Ranging.startAsTag(TAG_ADD,DW1000.MODE_LONGDATA_RANGE_LOWPOWER,DW1000.CHANNEL_7,false);
    } else if(channel1 == "1") {
      DW1000Ranging.startAsTag(TAG_ADD,DW1000.MODE_LONGDATA_RANGE_LOWPOWER,DW1000.CHANNEL_1,false);
    }

    delay(500);
    startTime = millis();
  }

  if(millis() - startTime2 > 300) {
    message = "COOR ALL ALL " + String(r1) + "," + String(r2);
    Serial2.println(message);
    message = "";
    startTime2 = millis();
  }
  
}