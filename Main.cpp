#include <Arduino.h>
#include <FastLED.h>
#include "projectDef.h"
#include "readLine.h"
#include "parseInput.h"
#include "processCmd.h"
#include "a_rtc.h"
#include "a_dht.h"
#include "a_fastLed.h"
#include "a_udp.h"
#include "a_lcd.h"

extern char outputFlag;

void setup() {
    Serial.begin(115200);
    //pinMode(t_D13, OUTPUT);
    pinMode(t_RED, OUTPUT);
    pinMode(t_GREEN, OUTPUT);
    //pinMode(t_RGB_RED, OUTPUT);
    //pinMode(t_RGB_GREEN, OUTPUT);
    //pinMode(t_RGB_BLUE, OUTPUT);
    a_fastLEDInit();
    rtcSetup();
    dhtDataInit();
    a_udpSetup();
    a_lcdInit();
}

void loop() {
  char* p;
  unsigned char* cmdbuf;
  p = serialReadLine();
  if (p){
    outputFlag = 0;
    cmdbuf = parseInput(p);
    if (cmdbuf){
      processCmd(cmdbuf);
    }
  }
  p = a_udpPacketReader();
  if (p){
    outputFlag = 1;
    cmdbuf = parseInput(p);
    if (cmdbuf){
      processCmd(cmdbuf);
    }
  }
  outputFlag = 0;
  blinkLoop();
  dhtLoop();
}