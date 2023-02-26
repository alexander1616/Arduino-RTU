#include <Arduino.h>
#include <SimpleDHT.h>
#include <EEPROM.h>
#include "projectDef.h"
#include "a_rtc.h"

extern DS3231_Simple Clock;
static char showTemp = 0;

float celToFahr(float cel){
    float fahr;
    fahr = (cel*(9/5)) + 32;
    return fahr;
}

typedef struct {
    unsigned int d_iterator;
    unsigned int d_count;
    char eleflag;
} dataInitElement_t;

dataInitElement_t dataInitElement;

void dhtDataInit(){
    char alexflag[4];
    EEPROM.get(0, alexflag);
    if ((alexflag[0] != 'a')||
            (alexflag[1] != 'l')||
            (alexflag[2] != 'e')||
            (alexflag[3] != 'x')){
        EEPROM.write(0, 'a');
        EEPROM.write(1, 'l');
        EEPROM.write(2, 'e');
        EEPROM.write(3, 'x');
        dataInitElement = {4 + sizeof(dataInitElement), 0, 0};
        EEPROM.put(4, &dataInitElement);
    } else {
        EEPROM.get(4, dataInitElement);
    }
}

typedef struct {
    unsigned long prevTimeRead;     //previous time last read
    unsigned long prevTimeStore;    //previous time last store
    unsigned long timeDelayRead;    //time delay before read
    unsigned long timeDelayStore;   //time delay before store
    unsigned int eepromlength;
} tempTimeElement_t;

static tempTimeElement_t tempTimeElement = {0, 0, 5000, 10000, EEPROM.length()};

typedef struct {
    float temperature;
    float humidity;
    DateTime datetime;
} tempHumidElement_t;

static unsigned char dht_dataFlag = 0;
static tempHumidElement_t tempHumidElement = {0.0, 0.0, {0}};

SimpleDHT22 dht22(t_DHT);

void dhtSetShowTemp(int flag){
    showTemp = flag;
};

int dhtReadTemp(tempHumidElement_t *ep){
  int err = SimpleDHTErrSuccess;
  float t_temp;
  float t_humid;
  if ((err = dht22.read2(&t_temp, &t_humid, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT22 failed, err="); Serial.print(SimpleDHTErrCode(err));
    Serial.print(","); Serial.println(SimpleDHTErrDuration(err));
    return -1;
  } else {
    dht_dataFlag = 1;
    ep->temperature = t_temp;
    ep->humidity = t_humid;
    ep->datetime = rtcGetTime();
    return 0;
  }
}

void dhtPrintTemp(tempHumidElement_t *ep) {
  Clock.printTo(Serial, tempHumidElement.datetime); Serial.print(F(" "));
  Serial.print(ep->temperature); Serial.print(F(" *C, "));
  Serial.print(celToFahr(ep->temperature)); Serial.print(F(" *F, "));
  Serial.print(ep->humidity); Serial.println(F(" RH%"));
}

void dhtShowTemp(){
    if (dht_dataFlag){
        dhtPrintTemp(&tempHumidElement);
    }
}

void dhtShowHistory(){
    if (dht_dataFlag){
        Serial.println("I'm going to write this");
    }
}

void dhtLoop(){
    unsigned long currentTime; //current time using millis
    unsigned long timeDiffRead; //compare value for read
    unsigned long timeDiffStore; //compare value for store
    currentTime = millis();
    timeDiffRead = currentTime - tempTimeElement.prevTimeRead;
    timeDiffStore = currentTime - tempTimeElement.prevTimeStore;
    if (timeDiffRead >= tempTimeElement.timeDelayRead){
        if(!dhtReadTemp(&tempHumidElement)){
            tempTimeElement.prevTimeRead = currentTime;
            if(showTemp){
                dhtShowTemp();
            }
        };
    }
    if (timeDiffStore >= tempTimeElement.timeDelayStore){
        Serial.println("Storing");
        //put code here for EEPROM
        if (dataInitElement.d_iterator + sizeof(tempHumidElement) > 
                tempTimeElement.eepromlength){
            dataInitElement.d_iterator = 4 + sizeof(dataInitElement);
            dataInitElement.eleflag = 1;
        }
        EEPROM.put(dataInitElement.d_iterator, &tempHumidElement);
        dataInitElement.d_iterator += sizeof(tempHumidElement);
        tempTimeElement.prevTimeStore = currentTime;
        if (!dataInitElement.eleflag){
            dataInitElement.d_count++;
        }
        EEPROM.put(4, &dataInitElement);
    }
}
