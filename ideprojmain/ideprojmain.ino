#include <Arduino.h>
#include "projectDef.h"
#include "readLine.h"

void setup(){
  Serial.begin(9600);
}

void loop() {  
  //Serial.println((char*)readLine());
  char* p;
  unsigned char* cmdbuf;
  p = serialReadLine();
  if (p){
    Serial.println(p);
    Serial.println("Done with process ");
  }
}
