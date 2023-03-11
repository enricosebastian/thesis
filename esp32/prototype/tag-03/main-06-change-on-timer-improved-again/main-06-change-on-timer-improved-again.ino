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

// tag 3
float t1 = 0.0;
float t2 = 0.0;
float r1 = 0.0;
float r2 = 0.0;

float prevT1 = t1;
float prevT2 = t2;

int correctTimes = 0;
int channel = 0;

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

  Serial.println("Starting Tag 3");

  DW1000Ranging.startAsTag(TAG_ADD,DW1000.MODE_LONGDATA_RANGE_LOWPOWER,DW1000.CHANNEL_1,false);
}

void loop() {
  if(millis() - startTime > 2000) {
    if(channel != 0) {
      channel = 0;
      DW1000Ranging.startAsTag(TAG_ADD,DW1000.MODE_LONGDATA_RANGE_LOWPOWER,DW1000.CHANNEL_1,false);
    } else if(channel == 0) {
      channel = 5;
      DW1000Ranging.startAsTag(TAG_ADD,DW1000.MODE_LONGDATA_RANGE_LOWPOWER,DW1000.CHANNEL_5,false);
    }
    startTime = millis();
  }

  if(channel != 0) {
    DW1000Ranging.loop();
    // For Anchor 1
    if(DW1000Ranging.getDistantDevice()->getShortAddress() == 0x1001 && channel == 5) {
      Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress());
      Serial.print(" - ");
      Serial.print(r1);
      Serial.print(",");
      Serial.println(r2);

      t1 = DW1000Ranging.getDistantDevice()->getRange();
      if(t1 > 0 && t1 < 30) {
        r1 = t1;
        correctTimes++;
      }

      if(correctTimes > 20) {
        DW1000Ranging.startAsTag(TAG_ADD,DW1000.MODE_LONGDATA_RANGE_LOWPOWER,DW1000.CHANNEL_7,false);
        Serial.println("Switching to channel 7");
        correctTimes = 0;
        channel = 7;
      }
    // For anchor 2
    } else if(DW1000Ranging.getDistantDevice()->getShortAddress() == 0x1002 && channel == 7) {

      Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress());
      Serial.print(" - ");
      Serial.print(r1);
      Serial.print(",");
      Serial.println(r2);
      
      t2 = DW1000Ranging.getDistantDevice()->getRange();
      if(t2 > 0 && t2 < 30) {
        r2 = t2;
        correctTimes++;
      }

      if(correctTimes > 20) {
        DW1000Ranging.startAsTag(TAG_ADD,DW1000.MODE_LONGDATA_RANGE_LOWPOWER,DW1000.CHANNEL_5,false);
        Serial.println("Switching to channel 5");
        correctTimes = 0;
        channel = 5;
      }
    }
  }

  if(millis() - startTime2 > 300) {

    
    // message = "COOR ALL ALL " + String(r1) + "," + String(r2);
    // Serial.println(message);
    // Serial2.println(message);
    // message = "";
    // startTime2 = millis();
  }
  
}