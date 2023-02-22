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
  //Serial.println((char*)readLine());
  char* p;
  unsigned char* cmdbuf;
  p = serialReadLine();
  if (p){
    //Serial.println(p);
    cmdbuf = parseInput(p);
    if (cmdbuf){
      Serial.println("We have a cmd buffer: ");
      processCmd(cmdbuf);
    }
    Serial.println("Done with process ");
  }
  blinkLoop();
}
