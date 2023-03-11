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
String channel1 = "1"; // drone 3
String channel2 = "1"; // none
String channel3 = "5"; // drone 1
String channel4 = "1"; // none
String channel5 = "7"; // drone 2
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

  Serial.println("Starting Tag 3");

  DW1000Ranging.startAsTag(TAG_ADD,DW1000.MODE_LONGDATA_RANGE_LOWPOWER,DW1000.CHANNEL_5,false);
}

void loop() {
  DW1000Ranging.loop();

  Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress());
  Serial.print(" - ");
  Serial.println(DW1000Ranging.getDistantDevice()->getRange());
  
}