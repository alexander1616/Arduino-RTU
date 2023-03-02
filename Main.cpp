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
}

void loop() {
  char* p;
  unsigned char* cmdbuf;
  p = serialReadLine();
  if (p){
    cmdbuf = parseInput(p);
    if (cmdbuf){
      processCmd(cmdbuf);
    }
  }
  p = a_udpPacketReader();
  if (p){
    cmdbuf = parseInput(p);
    if (cmdbuf){
      processCmd(cmdbuf);
    }
  }
  blinkLoop();
  dhtLoop();
}
