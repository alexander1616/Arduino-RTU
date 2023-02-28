#include <Arduino.h>
#include <FastLED.h>
#include "projectDef.h"

/*************************************************
*      Addressable RGB LED Configuration         *
*                                                *
*    Configures RGB LED(s)                       *
*    Uses FastLED Library function calls         *
*************************************************/

static CRGB ledsX[1];

void a_fastLEDInit(){
    FastLED.addLeds<WS2812, t_RGB_X, GRB>(ledsX, 1);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 500);
    FastLED.clear();
    FastLED.show();
}

void a_fastLEDSetVal(unsigned int val1, unsigned int val2, unsigned int val3){
    ledsX[0] = CRGB(val1, val2, val3);
    FastLED.setBrightness(20);
    FastLED.show();
}