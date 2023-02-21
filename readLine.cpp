#include <Arduino.h>
#include "readLine.h"

typedef unsigned char uchar_t;
#define BUFFSIZE 20

uchar_t testbuf[BUFFSIZE+1];
uchar_t buffer_size = 0;

int readLineBufSize(){
    return buffer_size;
}

uchar_t* readLine(){
  /*destroys buffer*/
  buffer_size = 0;
  int c;
  Serial.println("Enter a line please. ");
  while(1){

//    while (!Serial.available()){;};
    c = Serial.read();  
    if (c == -1){
      continue;
    }
    if (c == '\r' || c == '\n'){
      testbuf[buffer_size] = 0;
      return testbuf;
    }
    testbuf[buffer_size++] = c;
    if (buffer_size >= BUFFSIZE){
      testbuf[buffer_size] = 0;
      return testbuf;
    }
  }
}