#include <SPI.h>
#include "DW1000Ranging.h"
#include "DW1000.h"

#define ANCHOR_ADD "02:10:5B:D5:A9:9A:E2:9C"

#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23

const uint8_t PIN_RST = 27;
const uint8_t PIN_IRQ = 34;
const uint8_t PIN_SS = 4;

uint16_t Adelay = 16533;

float anch1;
float tmp;

uint16_t runtime1;

void setup() {
  // put your setup code here, to run once:
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ);
  DW1000.setAntennaDelay(Adelay);
  DW1000Ranging.attachNewRange(0);
  DW1000Ranging.attachBlinkDevice(0);
  DW1000Ranging.attachInactiveDevice(0);
  DW1000Ranging.startAsAnchor(ANCHOR_ADD, DW1000.MODE_LONGDATA_FAST_ACCURACY, DW1000.CHANNEL_7,false);
}

void loop() {
  // put your main code here, to run repeatedly:
   DW1000Ranging.loop();
   //if(digitalRead(12) == 1 && DW1000Ranging.getDistantDevice()->getShortAddress() == 0x1001) anch1Dist();
}

void anch1Dist(){
  int count = 0;
  float sum1 = 0;
  if(DW1000Ranging.getDistantDevice()->getRange() > 0){
    runtime1 = millis();
    while(count < 5){
      if((millis() - runtime1) > 1000){
        tmp = DW1000Ranging.getDistantDevice()->getRange();
        if(tmp > 0){
          sum1 += tmp;
          count++;
        }
        runtime1 = millis();
      }
    }
  }
  anch1 = sum1/count;
  DW1000Ranging.startAsAnchor(ANCHOR_ADD,DW1000.MODE_LONGDATA_RANGE_LOWPOWER,DW1000.CHANNEL_5,false);
}
