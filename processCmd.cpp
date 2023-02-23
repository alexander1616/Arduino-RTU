#include <Arduino.h>
#include "projectDef.h"

/*************************************************
*             Process Command                    *
*                                                *
*    Executes commands in sequence               *
*    Viable command sequences defined in help    *
*************************************************/

//Structure to store function variables
typedef struct {
  int red;
  int green;
  int colorstore;
  int d13BlinkSwitch;
  int ledBlinkSwitch;
  int rgblink;
  unsigned long timedelay;
  unsigned long timestart;
  unsigned long timestart2;
  unsigned long timestart3;
} prjDefault_t;

prjDefault_t prjDefault = {LOW, LOW, 0, 0, 0, 0, 500, 0, 0, 0};

//blink functions for d13 and led
void d13blink(){
    unsigned long timecurrent;
    timecurrent = millis();
    unsigned long tdelay;
    tdelay = timecurrent - prjDefault.timestart;
    if (tdelay >= prjDefault.timedelay){
        digitalRead(13)?digitalWrite(13, LOW):digitalWrite(13, HIGH);
        prjDefault.timestart = timecurrent;
    }
}

void ledBlink(){
    unsigned long timecurrent;
    timecurrent = millis();
    unsigned long tdelay;
    tdelay = timecurrent - prjDefault.timestart2;
    if (tdelay >= prjDefault.timedelay){     
        if (prjDefault.colorstore == 0){
            digitalWrite(t_GREEN, LOW);
            digitalRead(t_RED)?(digitalWrite(t_RED, LOW)):(digitalWrite(t_RED, HIGH));
        } else {
            digitalWrite(t_RED, LOW);
            digitalRead(t_GREEN)?(digitalWrite(t_GREEN, LOW)):(digitalWrite(t_GREEN, HIGH));
        }
        prjDefault.timestart2 = timecurrent;
    }
}

void rgBlink(){
    unsigned long timecurrent;
    timecurrent = millis();
    unsigned long tdelay;
    tdelay = timecurrent - prjDefault.timestart3;
    if (tdelay >= prjDefault.timedelay){
        if (digitalRead(t_RED)){
            digitalWrite(t_RED, LOW);
            digitalWrite(t_GREEN, HIGH);
        } else {
            digitalWrite(t_GREEN, LOW);
            digitalWrite(t_RED, HIGH);
        }
        prjDefault.timestart3 = timecurrent;
    }
}

void blinkLoop(){
  if (prjDefault.d13BlinkSwitch){
    d13blink();
  }
  if (prjDefault.ledBlinkSwitch){
    if (prjDefault.rgblink){
        rgBlink();
    } else {
        ledBlink();
    }
  }
};

//processing d13 pin and led pins
void d13Process(unsigned char arg){
  switch (arg){
  case t_ON:
    prjDefault.d13BlinkSwitch = 0;
    digitalWrite(13, HIGH);
    break;
  case t_OFF:
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
    prjDefault.ledBlinkSwitch = 0;
    prjDefault.rgblink = 0;
    digitalWrite(t_GREEN, prjDefault.green = LOW);
    digitalWrite(t_RED, prjDefault.red = HIGH);
    prjDefault.colorstore = 0;
    break;
  case t_GREEN:
    prjDefault.ledBlinkSwitch = 0;
    prjDefault.rgblink = 0;
    digitalWrite(t_RED, prjDefault.red = LOW);
    digitalWrite(t_GREEN, prjDefault.green = HIGH);
    prjDefault.colorstore = 1;
    break;
  case t_BLINK:
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

void processCmd(unsigned char* cmdbuf){
    unsigned char* p = cmdbuf;
    unsigned char cmd, arg1, arg2;
    while ((cmd = *p++) != t_EOL){
        char buf[30];
        // snprintf(buf, 30, "Process Command[%i]", cmd);
        // Serial.println(buf);
        switch (cmd) {
        case t_VERSION:
            Serial.println(F("Program Version 1.0"));
            break;
        case t_HELP:
            Serial.println(F("************************************************************"));
            Serial.println(F("* Supported CMDs [cmd][arg1][arg2] - command sequence      *"));
            Serial.println(F("************************************************************"));
            Serial.println(F("* D13     [cmd]             - access digital pin 13        *"));
            Serial.println(F("*   ON         [arg1]       - turns digital pin on         *"));
            Serial.println(F("*   OFF        [arg1]       - turns digital pin off        *"));
            Serial.println(F("* LED     [cmd]             - access dual LED pins         *"));
            Serial.println(F("*   GREEN      [arg1]       - turns green LED on/off       *"));
            Serial.println(F("*   RED        [arg1]       - turns red LED on/off         *"));
            Serial.println(F("*   OFF        [arg1]       - turns led pins off           *"));
            Serial.println(F("*   BLINK      [arg1]       - default 500 ms               *"));
            Serial.println(F("*       RG           [arg2] - r-g blinking for dual LED    *"));
            Serial.println(F("* SET     [cmd]             - sets blink interval          *"));
            Serial.println(F("*   BLINK      [arg1]       - default 500 ms               *"));
            Serial.println(F("*       500          [arg2] - configurable value for delay *"));
            Serial.println(F("* STATUS  [cmd]             - status menu                  *"));
            Serial.println(F("*   LEDS       [arg1]       - information about LEDs       *"));
            Serial.println(F("* VERSION [cmd]             - current program version      *"));
            Serial.println(F("* HELP    [cmd]              - displays help menu          *"));
            Serial.println(F("************************************************************"));
            break;
        case t_D13:
            arg1 = *p++;
            switch (arg1) {
            case t_ON:
                break;
            case t_OFF:
                break;
            case t_BLINK:
                break;
            default:
                Serial.println(F("Bad parameter for D13"));
                return;
            }
            d13Process(arg1);
            break;
        case t_LED:
            arg1 = *p++;
            switch (arg1) {
            case t_RED:
                break;
            case t_GREEN:
                break;
            case t_OFF:
                break;
            case t_BLINK:
                arg2 = *p;
                if(arg2 == t_RG){
                    prjDefault.rgblink = 1;
                    p++;
                }
                break;
            default:
                Serial.println(F("Bad parameter for LED"));
                return;
            }
            ledProcess(arg1);
            break;
        case t_SET:
            int highb;
            int lowb;
            arg1 = *p++;
            if (arg1 == t_BLINK){
                arg2 = *p++;
                if (arg2 == t_WORD){
                    highb = *p++;
                    lowb = *p++;
                    prjDefault.timedelay = (highb<<8)|(lowb);
                    if (prjDefault.timedelay > 10000){
                        Serial.println(F("Error, delay cannot exceed 10000ms"));
                        return;
                    }
                    Serial.print(F("Interval is set: "));
                    Serial.println(prjDefault.timedelay);
                } else {
                    Serial.println(F("Bad parameter for interval"));
                    return;
                }
            } else {
                Serial.println(F("Bad parameter for SET"));
                return;
            }
            break;
        case t_STATUS:
            arg1 = *p++;
            if (arg1 == t_LEDS){
                char buf[50];
                snprintf(buf, 50, "d13[%d], Red[%d], Green[%d]",
                    digitalRead(t_D13), digitalRead(t_RED), digitalRead(t_GREEN));
                Serial.println(buf);
            } else {
                Serial.println(F("Bad parameter for status"));
                return;
            }
            break;
        default:
            Serial.println(F("Not viable commands"));
            break;
        }
    }
}