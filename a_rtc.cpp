#include <Arduino.h>
#include <DS3231_Simple.h>

DS3231_Simple Clock;

void rtcSetTime(const DateTime & val){
    Clock.write(val);
}

DateTime rtcGetTime(){
    return Clock.read();
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

void rtcSetup() {
  Clock.begin();
}
