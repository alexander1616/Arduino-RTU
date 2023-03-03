#include <Arduino.h>
#include "projectDef.h"
#include "a_rtc.h"
#include "a_dht.h"
#include "a_fastLed.h"
#include "a_udp.h"
#define P_VERSION "V3"
#define HELP_1 "CMD RGB/LED/SET/TEMP/ADD/CLOCK/VERSION/HELP/STATUS"
#define HELP_2 "ARG ON/OFF/BLINK/LEDS/RGB/NUM"

/*************************************************
*             Process Command                    *
*                                                *
*    Executes commands in sequence               *
*    Viable command sequences defined in help    *
*************************************************/

char outputFlag = 0;
extern char udpBuf[];

//Structure to store function variables
typedef struct {
  int red;
  int green;
  int colorstore;
//  int d13BlinkSwitch;
  int ledBlinkSwitch;
  int rgblink;
  int rgbBlinkSwitch;
  int rgbBlink;
  unsigned long timedelay;
//  unsigned long timestart;
  unsigned long timestart2;
  unsigned long timestart3;
  unsigned long timestart4;
  unsigned char rgbRed; //0-255
  unsigned char rgbGreen; //0-255
  unsigned char rgbBlue; //0-255
  unsigned char rgbKey;
  unsigned long timestart5;
  //unsigned int ledByteVal; //byte value used for rotation
  //unsigned char ledByteSwitch; //flag control for byte blink
  unsigned char rgbCheck;
} prjDefault_t;

prjDefault_t prjDefault = {LOW, LOW, 0, /*0,*/ 0, 0, 0, 0, 500, 
                            /*0,*/ 0, 0, 0, 66, 227, 245, 0, 
                            0, /*0, 0,*/ 0};

//blink functions for d13 and led
#if 0
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
#endif
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

void rgbSetValue(unsigned int val1, unsigned int val2, unsigned int val3){
    prjDefault.rgbRed = (unsigned char)val1;
    prjDefault.rgbGreen = (unsigned char)val2;
    prjDefault.rgbBlue = (unsigned char)val3;
    // analogWrite(t_RGB_RED, prjDefault.rgbRed);
    // analogWrite(t_RGB_GREEN, prjDefault.rgbGreen);
    // analogWrite(t_RGB_BLUE, prjDefault.rgbBlue);
    a_fastLEDSetValX(prjDefault.rgbRed, prjDefault.rgbGreen, prjDefault.rgbBlue);
    prjDefault.rgbCheck = 1;
}

void rgbBlink(){
    unsigned long timecurrent;
    timecurrent = millis();
    unsigned long tdelay;
    tdelay = timecurrent - prjDefault.timestart4;
    if (tdelay >= prjDefault.timedelay){
        if (!prjDefault.rgbKey){
            // analogWrite(t_RGB_RED, prjDefault.rgbRed);
            // analogWrite(t_RGB_GREEN, prjDefault.rgbGreen);
            // analogWrite(t_RGB_BLUE, prjDefault.rgbBlue);
            a_fastLEDSetValX(prjDefault.rgbRed, prjDefault.rgbGreen, prjDefault.rgbBlue);
            prjDefault.rgbKey = 1;
            prjDefault.rgbCheck = 1;
        } else {
            // analogWrite(t_RGB_RED, 0);
            // analogWrite(t_RGB_GREEN, 0);
            // analogWrite(t_RGB_BLUE, 0);
            a_fastLEDSetValX(0, 0, 0);
            prjDefault.rgbKey = 0;
            prjDefault.rgbCheck = 0;
        }
        prjDefault.timestart4 = timecurrent;
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

void twoBitState(unsigned int xval){  
    switch (xval){
    case 0:
        digitalWrite(5, LOW);
        digitalWrite(6, LOW);
        break;
    case 1:
        digitalWrite(5, HIGH);
        digitalWrite(6, LOW);
        break;
    case 2:
        digitalWrite(6, HIGH);
        digitalWrite(5, LOW);
        break;
    case 3:
        digitalWrite(5, LOW);
        digitalWrite(5, LOW);
        break;
    default:
        break;
    }
}

static int bitSetLEDHelper_state = 1; //rotating from 1-7

// void bitSetLEDHelper(){
//     unsigned char xval;
//     switch (bitSetLEDHelper_state){
//     case 4:
//         xval = prjDefault.ledByteVal>>6&0x3;
//         twoBitState(xval);
//         bitSetLEDHelper_state++;
//         break;
//     case 5:
//         xval = prjDefault.ledByteVal>>4&0x3;
//         twoBitState(xval);
//         bitSetLEDHelper_state++;
//         break;
//     case 6:
//         xval = prjDefault.ledByteVal>>2&0x3;
//         twoBitState(xval);
//         bitSetLEDHelper_state++;
//         break;
//     case 7:
//         xval = prjDefault.ledByteVal&0x3;
//         twoBitState(xval);
//         bitSetLEDHelper_state = 1;
//         break;
//     case 3:
//         xval = prjDefault.ledByteVal>>8&0x3;
//         twoBitState(xval);
//         bitSetLEDHelper_state++;
//         break;
//     case 2:
//         xval = prjDefault.ledByteVal>>10&0x3;
//         twoBitState(xval);
//         bitSetLEDHelper_state++;
//         break;
//     case 1:
//         xval = prjDefault.ledByteVal>>12&0x3;
//         twoBitState(xval);
//         bitSetLEDHelper_state++;
//         break;
//     case 0:
//         xval = prjDefault.ledByteVal>>14&0x3;
//         twoBitState(xval);
//         bitSetLEDHelper_state++; 
//         break;
//     }
// }

// void bitSetLED(){
//     unsigned long timecurrent;
//     timecurrent = millis();
//     unsigned long tdelay;
//     tdelay = timecurrent - prjDefault.timestart5;
//     if (tdelay >= prjDefault.timedelay){   
//         bitSetLEDHelper();
//         prjDefault.timestart5 = timecurrent;
//     }
// }

void blinkLoop(){
#if 0
    if (prjDefault.d13BlinkSwitch){
        d13blink();
    }
#endif
    // if (!prjDefault.ledByteSwitch){
        if (prjDefault.ledBlinkSwitch){
            if (prjDefault.rgblink){
                rgBlink();
            } else {
                ledBlink();
            }
        }
    // } else {
    //     bitSetLED();
    // }
    if (prjDefault.rgbBlinkSwitch){
        rgbBlink();
    }
}

//processing d13 pin, led pins, and rgb pins
#if 0
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
#endif

void ledProcess(unsigned char arg1){
  switch (arg1){
  case t_RED:
    prjDefault.ledBlinkSwitch = 0;
    prjDefault.rgblink = 0;
    //prjDefault.ledByteSwitch = 0;
    digitalWrite(t_GREEN, prjDefault.green = LOW);
    digitalWrite(t_RED, prjDefault.red = HIGH);
    prjDefault.colorstore = 0;
    break;
  case t_GREEN:
    prjDefault.ledBlinkSwitch = 0;
    prjDefault.rgblink = 0;
    //prjDefault.ledByteSwitch = 0;
    digitalWrite(t_RED, prjDefault.red = LOW);
    digitalWrite(t_GREEN, prjDefault.green = HIGH);
    prjDefault.colorstore = 1;
    break;
  case t_BLINK:
    //prjDefault.ledByteSwitch = 0;
    prjDefault.ledBlinkSwitch = 1;
    break;
  case t_OFF:
    prjDefault.ledBlinkSwitch = 0;
    prjDefault.rgblink = 0;
    //prjDefault.ledByteSwitch = 0;
    digitalWrite(t_RED, prjDefault.red = LOW);
    digitalWrite(t_GREEN, prjDefault.green = LOW);
    break;
  }
}

void rgbProcess(unsigned char arg){
  switch (arg){
  case t_ON:
    prjDefault.rgbBlinkSwitch = 0;
    // analogWrite(t_RGB_RED, prjDefault.rgbRed);
    // analogWrite(t_RGB_GREEN, prjDefault.rgbGreen);
    // analogWrite(t_RGB_BLUE, prjDefault.rgbBlue);
    a_fastLEDSetValX(prjDefault.rgbRed, prjDefault.rgbGreen, prjDefault.rgbBlue);
    prjDefault.rgbCheck = 1;
    break;
  case t_OFF:
    prjDefault.rgbBlinkSwitch = 0;
    // analogWrite(t_RGB_RED, 0);
    // analogWrite(t_RGB_GREEN, 0);
    // analogWrite(t_RGB_BLUE, 0);
    a_fastLEDSetValX(0, 0, 0);
    prjDefault.rgbCheck = 0;
    break;
  case t_BLINK:
    prjDefault.rgbBlinkSwitch?(prjDefault.rgbBlinkSwitch = 0):(prjDefault.rgbBlinkSwitch = 1);
    break;
  }
};

int extractNum(unsigned char* cp){
    int value;
    value = (cp[0]<<8)|cp[1];
    return value;
}

void processCmd(unsigned char* cmdbuf){
    //char buf[50];
    unsigned char* p = cmdbuf;
    unsigned char cmd, arg1, arg2;
    while ((cmd = *p++) != t_EOL){
        switch (cmd) {
        case t_VERSION:
            if (outputFlag){
                a_udpSendAlert(P_VERSION);
            } else {
                Serial.println(P_VERSION);
            }
            break;
        case t_HELP:
            if (outputFlag){
                a_udpSendAlert(HELP_1);
                a_udpSendAlert(HELP_2);
            } else {
                Serial.println(HELP_1);
                Serial.println(HELP_2);
            }
            break;
/*
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
*/
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
            // case t_WORD:
            //     unsigned int ledvalue;
            //     ledvalue = extractNum(p);
            //     p+=2;
            //     prjDefault.ledByteVal = ledvalue; //0xff if mask for 4 state;
            //     prjDefault.ledByteSwitch = 1;
            //     //Serial.print(F("LED "));
            //     Serial.println(prjDefault.ledByteVal, BIN);
            //     return;
            default:
                Serial.println(F("3"));
                return;
            }
            ledProcess(arg1);
            break;
        case t_RGB:
            arg1 = *p++;
            switch (arg1) {
            case t_ON:
                break;
            case t_OFF:
                break;
            case t_BLINK:
                break;
            case t_WORD:
                unsigned int value1, value2, value3;
                value1 = extractNum(p);
                p+=2;
                if (*p == t_WORD){
                    p++;
                    value2 = extractNum(p);
                    p+=2;   
                } else {
                    Serial.println(F("4"));
                    return;
                }
                if (*p == t_WORD){
                    p++;
                    value3 = extractNum(p);
                    p+=2;   
                } else {
                    Serial.println(F("4"));
                    return;
                }
                if ((value1 > 255) || (value2 > 255) || (value3 > 255)){
                    Serial.println(F("5"));
                    return;
                } else {
                    prjDefault.rgbBlinkSwitch = 0;
                    rgbSetValue(value1, value2, value3);
                }
                break;
            default:
                Serial.println(F("4"));
                return;
            }
            rgbProcess(arg1);
            break;
        case t_CLOCK:
            rtcShowTime();
            break;
        case t_TEMP:
            arg1 = *p++;
            if (arg1 == t_HISTORY){
                dhtShowHistory();
            } else if (arg1 == t_ON){
                dhtSetShowTemp(1);    
            } else if (arg1 == t_OFF){
                dhtSetShowTemp(0);
            } else if (arg1 == t_MAX){
                dhtShowMaxTemp();
            } else if (arg1 == t_MIN){
                dhtShowMinTemp();
            } else if (arg1 == t_SHOW){
                dhtShowTemp();
            } else {
                Serial.println(F("6"));
            }
            break;
        case t_ADD:
            arg1 = *p++;
            int value1, value2;
            long result;
            if (arg1 == t_WORD){
                value1 = extractNum(p);
                p+=2;
            } else if (arg1 == t_NEG){
                arg1 = *p++;
                if (arg1 == t_WORD){
                    value1 = -(extractNum(p));
                    p+=2;
                }
            } else {
                Serial.println(F("7"));
                return;
            }
            arg2 = *p++;
            if (arg2 == t_WORD){
                value2 = extractNum(p);
                p+=2;
            } else if (arg2 == t_NEG){
                arg2 = *p++;
                if (arg2 == t_WORD){
                    value2 = -(extractNum(p));
                    p+=2;
                }
            } else {
                Serial.println(F("7"));
                return;
            }
            result = (long)value1 + (long)value2;
            
            snprintf(udpBuf, 30, "%d+%d=%ld", value1, value2, result);
            if (outputFlag){
                a_udpSendAlert(udpBuf);
            } else {
                // Serial.print(value1);
                // Serial.print(F(" + "));
                // Serial.print(value2);
                // Serial.print(F(" = "));
                // Serial.println(result);
                Serial.println(udpBuf);
            }
            break;
        case t_SET:
            //int highb;
            //int lowb;
            arg1 = *p++;
            if (arg1 == t_BLINK){
                arg2 = *p++;
                if (arg2 == t_WORD){
                    //highb = *p++;
                    //lowb = *p++;
                    //prjDefault.timedelay = (highb<<8)|(lowb);
                    prjDefault.timedelay = extractNum(p);
                    p+=2;
                    if (prjDefault.timedelay > 10000){
                        Serial.println(F("8"));
                        return;
                    }
                    Serial.print(F("Set "));
                    Serial.println(prjDefault.timedelay);
                } else {
                    Serial.println(F("9"));
                    return;
                }
            } else if (arg1 == t_CLOCK){ //define tclock
                //rtcPromptSetTime();
                //rtcInputTime();
                unsigned int year, month, day, hour, min, sec, dow;
                if (*p == t_WORD){
                    p++;
                    year = extractNum(p);
                    p+=2;   
                } else {
                    Serial.println(F("10"));
                    return;
                }
                if (*p == t_WORD){
                    p++;
                    month = extractNum(p);
                    p+=2;   
                } else {
                    Serial.println(F("11"));
                    return;
                }
                if (*p == t_WORD){
                    p++;
                    day = extractNum(p);
                    p+=2;   
                } else {
                    Serial.println(F("12"));
                    return;
                }
                if (*p == t_WORD){
                    p++;
                    hour = extractNum(p);
                    p+=2;   
                } else {
                    Serial.println(F("13"));
                    return;
                }
                if (*p == t_WORD){
                    p++;
                    min = extractNum(p);
                    p+=2;   
                } else {
                    Serial.println(F("14"));
                    return;
                }
                if (*p == t_WORD){
                    p++;
                    sec = extractNum(p);
                    p+=2;   
                } else {
                    Serial.println(F("15"));
                    return;
                }
                if (*p == t_WORD){
                    p++;
                    dow = extractNum(p);
                    p+=2;   
                } else {
                    Serial.println(F("26"));
                    return;
                }
                if ((year > 199) || (year < 0)){
                    Serial.println(F("16"));
                    return;
                } else if ((month > 12) || (month < 1)){
                    Serial.println(F("17"));
                    return;
                } else if ((day > 31) || (day < 1)){
                    Serial.println(F("18"));
                    return;
                } else if ((hour > 23) || (hour < 0)){
                    Serial.println(F("19"));
                    return;
                } else if ((min > 60) || (min < 0)){
                    Serial.println(F("20"));
                    return;
                } else if ((sec > 59) || (sec < 0)){
                    Serial.println(F("21"));
                    return;
                } else if ((dow > 7) || (dow < 1)){
                    Serial.println(F("27"));
                    return;
                } else {
                    rtcInputTime(year, month, day, hour, min, sec, dow);
                }
                break;
            } else if (arg1 == t_EEPROM){
                dhtEEPromInit();
                break;
            } else {
                Serial.println(F("22"));
                return;
            }
            break;
        case t_ALERT:
            a_udpSendAlert("alert!");
            break;
        case t_STATUS:
            arg1 = *p++;
            if (arg1 == t_LEDS){
                int red, green;
                red = digitalRead(t_RED);
                green = digitalRead(t_GREEN);
                Serial.print(F("R "));
                Serial.print(red);
                Serial.print(F(" G "));
                Serial.println(green);
                // snprintf(buf, sizeof(buf), "Red[%d], Green[%d]", /*removed d13*/
                //     digitalRead(t_RED), digitalRead(t_GREEN));
                // Serial.println(buf);
            } else if (arg1 == t_EEPROM){
                dhtShowEEProm();
            } else if (arg1 == t_RGB){
                char rgb = prjDefault.rgbCheck;
                Serial.print(F("RGB "));
                Serial.println(rgb);
                // snprintf(buf, sizeof(buf), "rgb [%d]", prjDefault.rgbCheck);
                // Serial.println(buf);
            } else {
                Serial.println(F("23"));
                return;
            }
            break;
        default:
            Serial.println(F("24"));
            break;
        }
    }
}