#include <Arduino.h>
#include "projectDef.h"
#include "readLine.h"
#include "parseInput.h"
#include "processCmd.h"

void setup() {
    Serial.begin(19200);
    pinMode(t_D13, OUTPUT);
    pinMode(t_RED, OUTPUT);
    pinMode(t_GREEN, OUTPUT);
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
  blinkLoop();
}