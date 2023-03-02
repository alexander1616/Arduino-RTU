#include <Arduino.h>
#include <SimpleDHT.h>
#include <EEPROM.h>
#include "projectDef.h"
#include "a_rtc.h"
#include "a_alarmStates.h"
#define TEMP_INTERVAL 5000
#define TEMP_STORE_INTERVAL 900000

extern DS3231_Simple Clock;
static char showTemp = 0;

/*************************************************
*      Humidity and Temperature Reader           *
*                                                *
*    Reads ambient temperature and humidity      *
*    Stores data as a "record" in EEPROM         *
*************************************************/

void dhtEEPromInit();

static float celToFahr(float cel){
    float fahr;
    fahr = (cel*(9.0/5.0)) + 32.0;
    return fahr;
}

//EEProm Header
typedef struct {
    unsigned int d_iterator;
    unsigned int d_count;
    char eleflag;
} dataInitElement_t;

//EEProm Record
typedef struct {
    float temperature;
    float humidity;
    DateTime datetime;
} tempHumidElement_t;

static dataInitElement_t dataInitElement;
static unsigned char dht_dataFlag = 0;
static tempHumidElement_t tempHumidElement = {0.0, 0.0, {0}};

inline int idx2adr(int idx) {
  int offset = 4+sizeof(dataInitElement);
  int ssize = sizeof(tempHumidElement);
  return offset + (idx * ssize);
}

static unsigned char checkMagic() {
  if ((EEPROM[0] == 'a') && (EEPROM[1] == 'l') && (EEPROM[2] == 'e') && (EEPROM[3] == 'x')) {
    return 0;  
  } else {
    return -1;
  }
}

static void dhtPrintInitElementHdr(dataInitElement_t & hdr) {
    char buf[50];
    snprintf(buf, sizeof(buf), " Hdr adr %u count %u eleflag %u record size %u",
             hdr.d_iterator,hdr.d_count, hdr.eleflag, sizeof(tempHumidElement_t));
    Serial.println(buf);
}

void dhtShowEEProm() {
  int   i;
  /* print magic */
  for (i = 0; i < 4; i++) {
    Serial.print((char)EEPROM[i]);
  }
  /* print hdr */
  dataInitElement_t hdr;
  EEPROM.get(4, hdr);
  dhtPrintInitElementHdr(hdr);


#if 0
  /* print first rec */
  int adr = idx2adr(0);
  EEPROM.get(adr, rec);
  snprintf(buf, sizeof(buf), "Record [0] adr %d ", adr);
  Serial.println(buf);
  dhtPrintTemp(&rec);
#endif
}

void dhtEEPromInit(){
    EEPROM[0] = 'a';
    EEPROM[1] = 'l';
    EEPROM[2] = 'e';
    EEPROM[3] = 'x';
    dataInitElement.d_count = 0;
    dataInitElement.d_iterator = idx2adr(0);
    dataInitElement.eleflag = 0;
    EEPROM.put(4, dataInitElement);
    dhtShowEEProm();
}

void dhtDataInit(){
    unsigned char magicReturn;
    magicReturn = checkMagic();
    //dhtShowEEProm();
    if (magicReturn){
        dhtEEPromInit();
    } else {
        EEPROM.get(4, dataInitElement);
        dhtShowEEProm();
    }
}

//Internal Data for Delays
typedef struct {
    unsigned long prevTimeRead;     //previous time last read
    unsigned long prevTimeStore;    //previous time last store
    unsigned long timeDelayRead;    //time delay before read
    unsigned long timeDelayStore;   //time delay before store
    unsigned int eepromlength;
} tempTimeElement_t;

static tempTimeElement_t tempTimeElement = {0, 0, 
                                                TEMP_INTERVAL, 
                                                TEMP_STORE_INTERVAL, 
                                                EEPROM.length()};

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
    char buf[50];
    snprintf(buf, sizeof(buf), "20%02d%02d%02d %02d%02d%02d ", 
                            tempHumidElement.datetime.Year,
                            tempHumidElement.datetime.Month,
                            tempHumidElement.datetime.Day,
                            tempHumidElement.datetime.Hour,
                            tempHumidElement.datetime.Minute,
                            tempHumidElement.datetime.Second);
    Serial.print(buf);
  //Clock.printTo(Serial, tempHumidElement.datetime); Serial.print(F(" "));
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
        if (!dataInitElement.eleflag){
            for (int i = idx2adr(0); 
                    i<idx2adr(dataInitElement.d_count); 
                    i+=sizeof(tempHumidElement)){
                EEPROM.get(i, tempHumidElement);
                dhtPrintTemp(&tempHumidElement);
            }
        } else {
            for (int i = dataInitElement.d_iterator; 
                    i<idx2adr(dataInitElement.d_count); 
                    i+=sizeof(tempHumidElement)){
                EEPROM.get(i, tempHumidElement);
                dhtPrintTemp(&tempHumidElement);
            }
            for (int i = idx2adr(0); 
                    i<dataInitElement.d_iterator;
                    i+=sizeof(tempHumidElement)){
                EEPROM.get(i, tempHumidElement);
                dhtPrintTemp(&tempHumidElement);
            }
        }
    }
}

void dhtLoop(){
    unsigned long currentTime; //current time using millis
    unsigned long timeDiffRead; //compare value for read
    unsigned long timeDiffStore; //compare value for store
    float ftemp;
    currentTime = millis();
    timeDiffRead = currentTime - tempTimeElement.prevTimeRead;
    timeDiffStore = currentTime - tempTimeElement.prevTimeStore;
    if (timeDiffRead >= tempTimeElement.timeDelayRead){
        if(!dhtReadTemp(&tempHumidElement)){
            tempTimeElement.prevTimeRead = currentTime;
            if(showTemp){
                dhtShowTemp();
            }
        }
        ftemp = celToFahr(tempHumidElement.temperature);
        if (ftemp <= 60.0){
            alarmTemperature(1);
        } else if ((ftemp > 60.0)&&(ftemp <= 70.0)){
            alarmTemperature(2);
        } else if ((ftemp > 70.0)&&(ftemp <= 80.0)){
            alarmTemperature(3);
        } else if ((ftemp > 80.0)&&(ftemp <= 90.0)){
            alarmTemperature(4);
        } else if (ftemp > 90.0){
            alarmTemperature(5);
        }
    }

    if (timeDiffStore >= tempTimeElement.timeDelayStore){
        //Serial.println("Storing");
        if (dataInitElement.d_iterator + sizeof(tempHumidElement)> 
                tempTimeElement.eepromlength){
            dataInitElement.d_iterator = idx2adr(0);
            dataInitElement.eleflag = 1;
        }
        EEPROM.put(dataInitElement.d_iterator, tempHumidElement);
        dataInitElement.d_iterator += sizeof(tempHumidElement);
        tempTimeElement.prevTimeStore = currentTime;
        if (!dataInitElement.eleflag){
            dataInitElement.d_count++;
        }
        EEPROM.put(4, dataInitElement);
        //dhtShowEEProm();
    }
}

void dhtShowMaxTemp(){
    if (!dht_dataFlag){
        return;
    }
    tempHumidElement_t compareElement;
    tempHumidElement_t storeElement;
    storeElement = {0.0, 0.0, {0}};
    for (int i=idx2adr(0); 
            i<idx2adr(dataInitElement.d_count); 
            i+=sizeof(tempHumidElement)){
        EEPROM.get(i, compareElement);
        if (compareElement.temperature > storeElement.temperature){
            storeElement.datetime = compareElement.datetime;
            storeElement.humidity = compareElement.humidity;
            storeElement.temperature = compareElement.temperature;
        }
    }
    dhtPrintTemp(&storeElement);
}

void dhtShowMinTemp(){
    if (!dht_dataFlag){
        return;
    }
    tempHumidElement_t compareElement;
    tempHumidElement_t storeElement;
    storeElement = {99.99, 0.0, {0}};
    for (int i=idx2adr(0); 
            i<idx2adr(dataInitElement.d_count); 
            i+=sizeof(tempHumidElement)){
        EEPROM.get(i, compareElement);
        if (compareElement.temperature < storeElement.temperature){
            storeElement.datetime = compareElement.datetime;
            storeElement.humidity = compareElement.humidity;
            storeElement.temperature = compareElement.temperature;
        }
    }
    dhtPrintTemp(&storeElement);
}