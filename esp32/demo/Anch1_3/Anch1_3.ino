#include <SPI.h>
#include "DW1000Ranging.h"
#include "DW1000.h"

#define ANCHOR_ADD "01:10:5B:D5:A9:9A:E2:9C"

#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23

const uint8_t PIN_RST = 27;
const uint8_t PIN_IRQ = 34;
const uint8_t PIN_SS = 4;

uint16_t Adelay = 16533;
uint32_t adr;

uint16_t runtime1;

float anch2 = 2.5;
float tmp;

void setup() {
  Serial.begin(9600);
  SPI.begin(SPI_SCK,SPI_MISO,SPI_MOSI);
  DW1000Ranging.initCommunication(PIN_RST,PIN_SS,PIN_IRQ);
  DW1000.setAntennaDelay(Adelay);
  DW1000Ranging.attachNewRange(0);
  DW1000Ranging.attachNewDevice(0);
  DW1000Ranging.attachInactiveDevice(0);
  DW1000Ranging.startAsAnchor(ANCHOR_ADD,DW1000.MODE_LONGDATA_FAST_ACCURACY,DW1000.CHANNEL_5,false);
}

void loop() {
  DW1000Ranging.loop();
  //if(digitalRead(12) == 1 && DW1000Ranging.getDistantDevice()->getShortAddress() == 0x1002) anch2Dist();
  //Serial.println(DW1000Ranging.getDistantDevice()->getRange());
}

void anch2Dist(){
  int count = 0;
  float sum1 = 0;
  int count1 = 0;
  //for(count1 = 0; count1 < 30; count1++)DW1000Ranging.loop();
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
  anch2 = sum1/count;
  DW1000Ranging.removeNetworkDevices(1);
  DW1000Ranging.startAsAnchor(ANCHOR_ADD,DW1000.MODE_LONGDATA_RANGE_LOWPOWER,DW1000.CHANNEL_5,false);
}
