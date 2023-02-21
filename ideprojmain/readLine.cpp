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

/*************************************************
 * serialReadLine                                *
 * 1) returns null if no input line detected     *
 * 2) returns pointer to null terminated buffer  *
 * 3) check terminating sequence \n \r BUFFSIZE  *
 *************************************************/
  // 2 global variables
  // cannot run multiple serial communication unless using object to track corresponding variables
uchar_t* serialReadLine(){
  int c;
  c = Serial.read();
  if (c == -1){
    return 0;
  }
  if (c == '\r' || c == '\n'){
    testbuf[buffer_size] = 0;
    buffer_size = 0;
    return testbuf;
  } 
  testbuf[buffer_size++] = c;
  if (buffer_size >= BUFFSIZE){
    testbuf[buffer_size] = 0;
    buffer_size = 0;
    return testbuf;
  }
  return 0;  
}
