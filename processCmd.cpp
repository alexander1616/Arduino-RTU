#include <Arduino.h>
#include "projectDef.h"

typedef struct {
  int interval;
  int red;
  int green;
  int colorstore;
  int d13BlinkSwitch;
  int ledBlinkSwitch;
  int rgblink;
} prjDefault_t;

prjDefault_t prjDefault = {500, LOW, LOW, 0, 0, 0, 0};
//unsigned long d13t1 = 0;

void blinkLoop(){
  if (prjDefault.d13BlinkSwitch){
    //Serial.println("d13 blink");
    // unsigned long d13t2;
    // d13t2 = millis();
    // if (){
    //     d13t1 = d13t2;
    // }
    digitalWrite(13, HIGH);
    delay(prjDefault.interval);
    digitalWrite(13, LOW);
    delay(prjDefault.interval);
  }
  if (prjDefault.ledBlinkSwitch){
    // Serial.print("blink colors: r/g: ");
    // Serial.print(prjDefault.red);
    // Serial.println(prjDefault.green);
    if ((prjDefault.red == LOW)&&(prjDefault.green == LOW)){
        if (prjDefault.colorstore){
            prjDefault.green = HIGH;
        } else {
            prjDefault.red = HIGH;
        }
        // Serial.print("Processing red: ");
        // Serial.print(prjDefault.red);
        // Serial.println(prjDefault.green);
    }
    digitalWrite(t_RED, prjDefault.red);
    digitalWrite(t_GREEN, prjDefault.green);
    delay(prjDefault.interval);
    prjDefault.red?(digitalWrite(t_RED, LOW)):(digitalWrite(t_GREEN, LOW));
    // if (prjDefault.red){
    //     digitalWrite(t_RED, LOW);
    // } else {
    //     digitalWrite(t_GREEN, LOW);
    // }
    delay(prjDefault.interval);
  }
  if (prjDefault.rgblink){
    prjDefault.ledBlinkSwitch = 0;
    digitalWrite(t_RED, HIGH);
    digitalWrite(t_GREEN, LOW);
    delay(prjDefault.interval);
    digitalWrite(t_RED, LOW);
    digitalWrite(t_GREEN, HIGH);
    delay(prjDefault.interval);
  }
};

void d13Process(unsigned char arg){
  switch (arg){
  case t_ON:
    //Serial.println("d13 on");
    prjDefault.d13BlinkSwitch = 0;
    digitalWrite(13, HIGH);
    break;
  case t_OFF:
    //Serial.println("d13 off");
    prjDefault.d13BlinkSwitch = 0;
    digitalWrite(13, LOW);
    break;
  case t_BLINK:
    prjDefault.d13BlinkSwitch?(prjDefault.d13BlinkSwitch = 0):(prjDefault.d13BlinkSwitch = 1);
    break;
  }
};

void ledProcess(unsigned char arg1){
  switch (arg1){
  case t_RED:
    //digitalWrite(t_RED, HIGH);
    digitalWrite(t_GREEN, prjDefault.green = LOW);
    //prjDefault.red?(prjDefault.red = LOW):(prjDefault.red = HIGH);
    // if (prjDefault.red == HIGH){
    //     prjDefault.red == LOW;
    // }
    digitalWrite(t_RED, prjDefault.red = HIGH);
    prjDefault.colorstore = 0;
    break;
  case t_GREEN:
    digitalWrite(t_RED, prjDefault.red = LOW);
    //prjDefault.green?(prjDefault.green = LOW):(prjDefault.green = HIGH);
    //digitalWrite(t_GREEN, prjDefault.green);
    digitalWrite(t_GREEN, prjDefault.green = HIGH);
    prjDefault.colorstore = 1;
    break;
  case t_BLINK:
    //prjDefault.ledBlinkSwitch?(prjDefault.ledBlinkSwitch = 0):(prjDefault.ledBlinkSwitch = 1);
    prjDefault.ledBlinkSwitch = 1;
    break;
  case t_OFF:
    prjDefault.ledBlinkSwitch = 0;
    prjDefault.rgblink = 0;
    digitalWrite(t_RED, prjDefault.red = LOW);
    digitalWrite(t_GREEN, prjDefault.green = LOW);
    break;
  }
}

// if ((prjDefault.red == LOW)&&(prjDefault.green == LOW)){
//                     prjDefault.red = HIGH;
//                 }
//                 delay(prjDefault.interval);
//                 digitalWrite(t_RED, prjDefault.red);
//                 digitalWrite(t_GREEN, prjDefault.green);
//                 delay(prjDefault.interval);

void processCmd(unsigned char* cmdbuf){
    unsigned char* p = cmdbuf;
    unsigned char cmd, arg1, arg2;
    while ((cmd = *p++) != t_EOL){
        char buf[30];
        snprintf(buf, 30, "Process Command[%i]", cmd);
        Serial.println(buf);
        switch (cmd) {
        case t_VERSION:
            Serial.println("Program Version 1.0");
            break;
        case t_HELP:
            Serial.println(F("******************************************"));
            Serial.println(F("*            Supported CMDs              *"));
            Serial.println(F("******************************************"));
            Serial.println(F("* D13       - access digital pin 13      *"));
            Serial.println(F("*   ON      - turns digital pin on       *"));
            Serial.println(F("*   OFF     - turns digital pin off      *"));
            Serial.println(F("* LED       - access dual LED pins       *"));
            Serial.println(F("*   GREEN   - turns green LED on/off     *"));
            Serial.println(F("*   RED     - turns red LED on/off       *"));
            Serial.println(F("*   BLINK   - default 500 ms             *"));
            Serial.println(F("*     RG    - r-g blinking for dual LED  *"));
            Serial.println(F("* SET       - sets blink interval        *"));
            Serial.println(F("*   BLINK   - default 500 ms             *"));
            Serial.println(F("* STATUS    - status menu                *"));
            Serial.println(F("*   LEDS    - information about LEDs     *"));
            Serial.println(F("* VERSION   - current program version    *"));
            Serial.println(F("* HELP      - displays help menu         *"));
            Serial.println(F("******************************************"));
            break;
        case t_D13:
            Serial.println("In d13");
            arg1 = *p++;
            d13Process(arg1);
            break;
        case t_LED:
            Serial.println("In LED");
            arg1 = *p++;
            arg2 = *p++;
            if(arg2 == t_RG){
                //prjDefault.rgblink?(prjDefault.rgblink = 0):(prjDefault.rgblink = 1);
                prjDefault.rgblink = 1;
            }
            ledProcess(arg1);
            break;
        case t_SET:
            int highb;
            int lowb;
            Serial.println("In Set");
            arg1 = *p++;
            if (arg1 == t_BLINK){
                arg2 = *p++;
                Serial.print("Arg2: should be word");
                Serial.println(arg2);
                if (arg2 == t_WORD){
                    highb = *p++;
                    lowb = *p++;
                    prjDefault.interval = (highb<<8)|(lowb);
                    Serial.print("Interval is set: ");
                    Serial.println(prjDefault.interval);
                }
            }
            break;
        case t_STATUS:
            Serial.println("In Status");
            arg1 = *p++;
            if (arg1 == t_LEDS){
                char buf[30];
                snprintf(buf, 30, "LEDs: Red[%d], Green[%d]", prjDefault.red, prjDefault.green);
                Serial.println(buf);
            }
            break;
        default:
            break;
        }
    }
};
