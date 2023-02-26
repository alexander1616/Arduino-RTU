#ifndef a_rtc_h
#define a_rtc_h 1

#include <DS3231_Simple.h>

extern void rtcSetup();
extern void rtcPromptSetTime();
extern void rtcShowTime();
extern DateTime rtcGetTime();

#endif