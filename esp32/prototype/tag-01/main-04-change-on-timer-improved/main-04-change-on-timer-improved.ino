#include <SPI.h>
#include "DW1000Ranging.h"
#include "DW1000.h"

#define TAG_ADD "01:20:5B:D5:A9:9A:E2:9C" // Tag 1

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
String channel1 = "5"; // drone 1
String channel2 = "1"; // none
String channel3 = "7"; // drone 2
String channel4 = "1"; // none
String channel5 = "1"; // drone 3
String channel6 = "1"; // none

float t1 = 0.0;
float t2 = 0.0;
float r1 = 0.0;
float r2 = 0.0;

float prevT1 = t1;
float prevT2 = t2;

unsigned long completedTime = 0;
unsigned long startTime = 0;
unsigned long startTime2 = 0;

bool hasStartedCounting = false;

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600,SERIAL_8N1,RX,TX);
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ);
  DW1000Ranging.attachNewRange(0);
  DW1000Ranging.attachNewDevice(0);
  DW1000Ranging.attachInactiveDevice(0);

  Serial.println("Starting Tag 1");

  DW1000Ranging.startAsTag(TAG_ADD,DW1000.MODE_LONGDATA_FAST_ACCURACY,DW1000.CHANNEL_1,false);
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

      // Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress());
      // Serial.print(" - ");
      // Serial.print(r1);
      // Serial.print(",");
      // Serial.println(r2);
    // For anchor 2
    } else if(DW1000Ranging.getDistantDevice()->getShortAddress() == 0x1002 && channel1 == "7") {
      
      t2 = DW1000Ranging.getDistantDevice()->getRange();
      if(t2 > 0 && t2 < 30) {
        r2 = t2;
      }

      // Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress());
      // Serial.print(" - ");
      // Serial.print(r1);
      // Serial.print(",");
      // Serial.println(r2);
    }
  }
  
  if((millis() - startTime > 500)) {
    tempChannel = channel1;
    channel1 = channel2;
    channel2 = channel3;
    channel3 = channel4;
    channel4 = channel5;
    channel5 = channel6;
    channel6 = tempChannel;

    // Serial.print("Changing to channel ");
    // Serial.println(channel1);

    if(channel1 == "5") {
      if(!hasStartedCounting) {
        completedTime = millis();
        hasStartedCounting = true;
        prevT1 = t1;
        prevT2 = t2;
      }
      else if(hasStartedCounting && !(t1 == prevT1 || t2 == prevT2)) {
        // Serial.print("Round time takes: ");
        // Serial.print((millis() - completedTime)/1000);
        // Serial.println(" seconds");

        // Serial.print(r1);
        // Serial.print(",");
        // Serial.println(r2);
        // Serial.println();
        hasStartedCounting = false;
      }
      DW1000Ranging.startAsTag(TAG_ADD,DW1000.MODE_LONGDATA_FAST_ACCURACY,DW1000.CHANNEL_5,false);
    } else if(channel1 == "7") {
      DW1000Ranging.startAsTag(TAG_ADD,DW1000.MODE_LONGDATA_FAST_ACCURACY,DW1000.CHANNEL_7,false);
    } else if(channel1 == "1") {
      DW1000Ranging.startAsTag(TAG_ADD,DW1000.MODE_LONGDATA_FAST_ACCURACY,DW1000.CHANNEL_1,false);
    }
    startTime = millis();
  }

  if(millis() - startTime2 > 300) {
    message = "COOR ALL ALL " + String(r1) + "," + String(r2);
    Serial.println(message);
    Serial2.println(message);
    message = "";
    startTime2 = millis();
  }
  
}