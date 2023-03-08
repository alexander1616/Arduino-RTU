#include <Arduino.h>
#include "a_analogButton.h"
#define ANALOG_PIN A0

/*************************************************
*         Analog Button Library                  *
*                                                *
*    Processes analog readings from button       *
*    Uses bit pattern to filter out noise        *
*************************************************/

//hardware interface
static char buttonKey(){
  unsigned int n, ns;
  char buf[100];
  unsigned char key;
  n = analogRead(ANALOG_PIN);
  ns = (n>>7) & 0x7;
//  snprintf(buf, sizeof(buf), "%x %d %x %d [%x %d]", 
//                              nx, nx, n, n, ns, ns);
  switch(ns){
  case 0x7:
    key = KEY_SW0;
    break;
  case 0:
    key = KEY_SW1;
    break;
  case 0x1:
    key = KEY_SW2;
    break;
  case 0x2:
    key = KEY_SW3;
    break;
  case 0x3:
    key = KEY_SW4;
    break;
  case 0x5:
    key = KEY_SW5;
    break;
  default:
    key = KEY_SWX;
    break;
  }
  return key;
}

void analogKey::clear(){
  for (int i = 0; i < 7; i++){
    keyBucket[i] = 0;
  }
}

char analogKey::rank() {
  char n;
  char store = 0;
  for (n = 1; n < 7; n++) {
    if (keyBucket[n] > keyBucket[store]) {
      store = n;
    }
  }
  return store;
}

char analogKey::getKey(){
  char key;
  key = buttonKey();
  if (key == KEY_SW0) {
    return key;
  }
  clear();
  saveKey(key);
  unsigned long keyFuture = 0;
  if (keyFuture == 0){
    keyFuture = millis() + 200;
  }
  while (millis() < keyFuture){
    saveKey(buttonKey());
  }
  return rank();
}

//Example usage in loop
// void loop() {
//   char key;
//   analogKey button;
//   key = button.getKey();
//   switch (key){
//   case KEY_SW0:
//     //Serial.println("Nothing pressed");
//     break;
//   case KEY_SW1:
//     Serial.println("[1] Pressed");
//     break;
//   case KEY_SW2:
//     Serial.println("[2] Pressed");
//     break;
//   case KEY_SW3:
//     Serial.println("[3] Pressed");
//     break;
//   case KEY_SW4:
//     Serial.println("[4] Pressed");
//     break;
//   case KEY_SW5:
//     Serial.println("[5] Pressed");
//     break;
//   case KEY_SWX:
//     //Serial.println("Unknown key");
//     break;
//   default:
//     Serial.println("Impossible!");
//     break;
//   }
// }