#include <Arduino.h>
#include "a_fastLed.h"
#include "a_udp.h"

/*************************************************
*         LED and UDP Alarm Signals              *
*                                                *
*    Processes ambient threshholds and           *
*    network conditions for UDP transmission     *
*************************************************/

char prevTemp = 0;

void alarmNetwork(char netErr){
    switch (netErr) {
    case -1:
        a_fastLEDSetVal(255, 0, 0);
        break;
    case -2:
        a_fastLEDSetVal(255, 0, 255);
        break;
    case 0:
        a_fastLEDSetVal(0, 0, 0);
        break;
    }
}

void alarmTemperature(char tval){
    if (prevTemp == tval){
        return;
    } else {
        switch (tval) {
        case 1:
            a_fastLEDSetVal1(255, 0, 255);
            a_udpSendAlert("freezing!");
            break;
        case 2:
            a_fastLEDSetVal1(0, 0, 255);
            a_udpSendAlert("cold");
            break;
        case 3:
            a_fastLEDSetVal1(0, 255, 0);
            a_udpSendAlert("comfortable");
            break;
        case 4:
            a_fastLEDSetVal1(100, 64, 0);
            a_udpSendAlert("hot");
            break;
        case 5:
            a_fastLEDSetVal1(255, 0, 0);
            a_udpSendAlert("fire!");
            break;
        }
        prevTemp = tval;
    }
}