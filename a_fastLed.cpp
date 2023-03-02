#include <Arduino.h>
#include <FastLED.h>
#include "projectDef.h"
#define NUM_LEDS_XYZ 3


/*************************************************
*      Addressable RGB LED Configuration         *
*                                                *
*    Configures RGB LED(s)                       *
*    Uses FastLED Library function calls         *
*************************************************/

static CRGB ledsXYZ[NUM_LEDS_XYZ];
static CRGB ledsX[1];

void a_fastLEDInit(){
    FastLED.addLeds<NEOPIXEL, t_RGB_XYZ>(ledsXYZ, 3);
    FastLED.addLeds<NEOPIXEL, t_RGB_X>(ledsX, 3);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 500);
    FastLED.clear();
    ledsXYZ[2] = CRGB(0, 200, 0);
    FastLED.setBrightness(20);
    FastLED.show();
}

void a_fastLEDSetVal(unsigned int val1, unsigned int val2, unsigned int val3){
    ledsXYZ[0] = CRGB(val1, val2, val3);
    //ledsX[1] = CRGB(val1, val2, val3);
    //ledsX[2] = CRGB(val1, val2, val3);
    FastLED.setBrightness(40);
    FastLED.show();
}

void a_fastLEDSetVal1(unsigned int val1, unsigned int val2, unsigned int val3){
    //ledsX[0] = CRGB(val1, val2, val3);
    ledsXYZ[1] = CRGB(val1, val2, val3);
    //ledsX[2] = CRGB(val1, val2, val3);
    FastLED.setBrightness(40);
    FastLED.show();
}

void a_fastLEDSetValX(unsigned int val1, unsigned int val2, unsigned int val3){
    ledsX[0] = CRGB(val1, val2, val3);
    FastLED.setBrightness(40);
    FastLED.show();
}