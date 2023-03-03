#include <Arduino.h>
#include <DS3231_Simple.h>

/*************************************************
*             Real Time Clock                    *
*                                                *
*    Functions to view and set time              *
*    Interfaces with records to store in EEPROM  *
*************************************************/

DS3231_Simple Clock;

void rtcSetTime(const DateTime & val){
    Clock.write(val);
}

DateTime rtcGetTime(){
    DateTime tClock;
    tClock = Clock.read();
    return (tClock);
}

void rtcShowTime(){
    Clock.printDateTo_YMD(Serial);
    Serial.print(' ');
    Clock.printTimeTo_HMS(Serial);
    Serial.println();
}

void rtcPromptSetTime(){
    Clock.promptForTimeAndDate(Serial);
}

void rtcInputTime(unsigned char yr, unsigned char mth, unsigned char day, 
                    unsigned char hr, unsigned char min, unsigned char sec,
                    unsigned char dow){
    DateTime tempTime;
    tempTime.Year = yr;
    tempTime.Month = mth;
    tempTime.Day = day;
    tempTime.Hour = hr;
    tempTime.Minute = min;
    tempTime.Second = sec;
    tempTime.Dow = dow;
    rtcSetTime(tempTime);
}

void rtcSetup() {
  Clock.begin();
}