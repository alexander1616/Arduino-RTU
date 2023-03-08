#include <Arduino.h>
#include <SimpleDHT.h>
#include <EEPROM.h>
#include "projectDef.h"
#include "a_rtc.h"
#include "a_alarmStates.h"
#include "a_udp.h"
#include "a_lcd.h"
#define TEMP_INTERVAL 5000
#define TEMP_STORE_INTERVAL 900000

extern DS3231_Simple Clock;
extern char outputFlag;
static char showTemp = 0;
char udpBuf[60] = {0};

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

//EEProm ip and threshhold vals
typedef struct {
    unsigned char ip[4];
    unsigned char mask[4];
    unsigned char gateway[4];
    int majorUnder;
    int minorUnder;
    int minorOver;
    int majorOver;
} ipAlarmElement_t;

ipAlarmElement_t ipAlarmElement = {
    {192, 168, 20, 10},
    {255, 255, 255, 0},
    {192, 168, 20, 1},
    60, 70, 80, 90
};

static dataInitElement_t dataInitElement;
unsigned char dht_dataFlag = 0;
static tempHumidElement_t tempHumidElement = {0.0, 0.0, {0}};

inline int idx2adr(int idx) {
  int offset = 4 + sizeof(ipAlarmElement) + sizeof(dataInitElement);
  int ssize = sizeof(tempHumidElement);
  return offset + (idx * ssize);
}

//kkk
int lastElementIdx(){
    int offset = dataInitElement.d_iterator - 4 - sizeof(ipAlarmElement) - sizeof(dataInitElement);
    int idx = offset/sizeof(tempHumidElement);
    if (idx == 0){
        if (!dataInitElement.eleflag){
            return -1;
        } else {
            return dataInitElement.d_count - 1;
        }
    } 
    return idx - 1;
}

static unsigned char checkMagic() {
  if ((EEPROM[0] == 'c') && (EEPROM[1] == 'l') && (EEPROM[2] == 'e') && (EEPROM[3] == 'x')) {
    return 0;  
  } else {
    return -1;
  }
}

static void dhtPrintInitElementHdr(dataInitElement_t & hdr) {
    Serial.print(F(" "));
    Serial.print(hdr.d_count);
    Serial.print(F(" "));
    Serial.println(sizeof(tempHumidElement_t));
}

// static void dhtPrintIPAlarmHdr(ipAlarmElement_t & alrm ){
//     int i = 0;
//     for (i = 0; i < 4; i++){
//         Serial.print(alrm.ip[i]);
//     }
//     Serial.print(F(" "));
//     for (i = 0; i < 4; i++){
//         Serial.print(alrm.mask[i]);
//     }
//     Serial.print(F(" "));
//     for (i = 0; i < 4; i++){
//         Serial.print(alrm.gateway[i]);
//     }
//     Serial.println(F(" "));
//     Serial.print(alrm.majorUnder);
//     Serial.print(F(" "));
//     Serial.print(alrm.minorUnder);
//     Serial.print(F(" "));
//     Serial.print(alrm.minorOver);
//     Serial.print(F(" "));
//     Serial.println(alrm.majorOver);
// }

void dhtShowEEProm() {
  int i;
  for (i = 0; i < 4; i++) {
    Serial.print((char)EEPROM[i]);
  }
  /* print alarm */
//   ipAlarmElement_t alrm;
//   EEPROM.get(4, alrm);
//   dhtPrintIPAlarmHdr(alrm);
  /* print hdr */
  dataInitElement_t hdr;
  EEPROM.get(4+sizeof(ipAlarmElement), hdr);
  dhtPrintInitElementHdr(hdr);
}

void saveIPAlarm(){
    EEPROM.put(4, ipAlarmElement);
}

void readIPAlarm(){
    EEPROM.get(4, ipAlarmElement);
}

void saveDataInit(){
    EEPROM.put(4 + sizeof(ipAlarmElement), dataInitElement);
}

void readDataInit(){
    EEPROM.get(4 + sizeof(ipAlarmElement), dataInitElement);
}

void readDataElement(tempHumidElement_t &value, int idx){
    EEPROM.get(idx2adr(idx), value);
}

void recordInit(){
    dataInitElement.d_count = 0;
    dataInitElement.d_iterator = idx2adr(0);
    dataInitElement.eleflag = 0;
    saveDataInit();
}

void dhtEEPromInit(){
    EEPROM[0] = 'c';
    EEPROM[1] = 'l';
    EEPROM[2] = 'e';
    EEPROM[3] = 'x';

    recordInit();
    // dataInitElement.d_count = 0;
    // dataInitElement.d_iterator = idx2adr(0);
    // dataInitElement.eleflag = 0;

    //Serial.println("In dhtEEPROM Init");
    saveIPAlarm();
    //saveDataInit();
    a_assignGateway(ipAlarmElement.gateway);
    a_assignMask(ipAlarmElement.mask);
    a_assignIP(ipAlarmElement.ip);
    dhtShowEEProm();
}

void dhtDataInit(){
    unsigned char magicReturn;
    magicReturn = checkMagic();
    //dhtShowEEProm();
    if (magicReturn){
        //Serial.println("in dhtdataInit magicReturn loop");
        dhtEEPromInit();
    } else {
        //Serial.println("In dhtDataInit else loop");
        readIPAlarm();
        readDataInit();
        a_assignGateway(ipAlarmElement.gateway);
        a_assignMask(ipAlarmElement.mask);
        a_assignIP(ipAlarmElement.ip);
        //Serial.println("assignIP from dhtdataInit");
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
    // Serial.print(F("dht22err=")); Serial.print(SimpleDHTErrCode(err));
    // Serial.print(F(",")); Serial.println(SimpleDHTErrDuration(err));
    return -1;
  } else {
    dht_dataFlag = 1;
    ep->temperature = t_temp;
    ep->humidity = t_humid;
    ep->datetime = rtcGetTime();
    return 0;
  }
}

char* temp2str(float temp, float humid, char* buf){
    int tdigit, tdec;
    int hdigit, hdec;
    tdigit = (int)temp;
    hdigit = (int)humid;
    tdec = ((int)(temp*10.0))%10;
    hdec = ((int)(humid*10.0))%10;
    snprintf(buf, 20, " %d.%dF %d.%dRH", tdigit, tdec, hdigit, hdec);
    return buf;
}

char* dt2str(DateTime &dt, char* buf){
    snprintf(buf, 21, "20%02d/%02d/%02d %02d:%02d:%02d ", 
                            dt.Year, dt.Month, dt.Day,
                            dt.Hour, dt.Minute, dt.Second);
    return buf;
}

void lcdPrintTemp(tempHumidElement_t *ep){
    char line1buf[17];
    char line2buf[17];
    int tdigit, tdec;
    int hdigit, hdec;
    float fahr;
    fahr = celToFahr(ep->temperature);
    tdigit = (int)fahr;
    hdigit = (int)ep->humidity;
    tdec = ((int)(fahr*10.0))%10;
    hdec = ((int)(ep->humidity*10.0))%10;
    snprintf(line1buf, 17, "%d.%dF %d.%dRH", tdigit, tdec, hdigit, hdec);
    snprintf(line2buf, 17, "%02d/%02d %02d:%02d", 
                            ep->datetime.Month, ep->datetime.Day,
                            ep->datetime.Hour, ep->datetime.Minute);
    a_lcdPrint(0, line2buf);
    a_lcdPrint(1, line1buf);
}

void lcdShowTemp(){
    if (dht_dataFlag){
        lcdPrintTemp(&tempHumidElement);
    }
}


void lcdPrintHistory(char param){
    static int store_history_idx = 0;
    if (!dataInitElement.d_count){
        a_lcdPrint(0, "No Data");
        a_lcdPrint(1, " ");
        return;
    }
    int idx = 0;
    tempHumidElement_t xEle;
    if (param == 0){
        idx = lastElementIdx();
    } else if (param == -1){
        idx = store_history_idx - 1;
    } else if (param == 1){
        idx = store_history_idx + 1;
    }
    if (idx <= -1){
        idx = dataInitElement.d_count - 1;
    } else if (idx >= dataInitElement.d_count){
        idx = 0;
    } 
    readDataElement(xEle, idx);
    lcdPrintTemp(&xEle);
    store_history_idx = idx;
}

void test_lcdPrintHistory(int move){
    if (!dataInitElement.d_count){
        a_lcdPrint(0, "No Data");
        a_lcdPrint(1, " ");
        return;
    }
    int idx = 0;
    tempHumidElement_t xEle;
    idx = lastElementIdx() + move;
    
    if (idx <= -1){
        idx = dataInitElement.d_count - 1;
    } else if (idx >= dataInitElement.d_count){
        idx = 0;
    }
    readDataElement(xEle, idx);
    lcdPrintTemp(&xEle);
}

void test_lcdShowHistory(int move){
    if (dht_dataFlag){
        test_lcdPrintHistory(move);
    }
}

void lcdShowHistory(char param){
    if (dht_dataFlag){
        lcdPrintHistory(param);
    }
}

void dhtPrintTemp(tempHumidElement_t *ep) {
    dt2str(tempHumidElement.datetime, udpBuf);
    temp2str(celToFahr(tempHumidElement.temperature), tempHumidElement.humidity, udpBuf+19);
    if (outputFlag){
        a_udpSendAlert(udpBuf);
    } else {
        Serial.println(udpBuf); 
    }
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
        ftemp = celToFahr(tempHumidElement.temperature);
        if ((int)ftemp <= ipAlarmElement.majorUnder){
            alarmTemperature(1);
        } else if (((int)ftemp > ipAlarmElement.majorUnder)&&((int)ftemp <= ipAlarmElement.minorUnder)){
            alarmTemperature(2);
        } else if (((int)ftemp > ipAlarmElement.minorUnder)&&((int)ftemp <= ipAlarmElement.minorOver)){
            alarmTemperature(3);
        } else if (((int)ftemp > ipAlarmElement.minorOver)&&((int)ftemp <= ipAlarmElement.majorOver)){
            alarmTemperature(4);
        } else /*if ((int)ftemp > ipAlarmElement.majorOver)*/{
            alarmTemperature(5);
        }
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
        saveDataInit();
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

#include "a_lcdTuple.h"
#include "a_lcdTemp.h"

void proc_initKeyState(char mode) {
    switch(mode) {
    case 101:
        lcdTupleData.setTuple(ipAlarmElement.ip);
        break;
    case 102:
        lcdTupleData.setTuple(ipAlarmElement.mask);
        break;
    case 103:
        lcdTupleData.setTuple(ipAlarmElement.gateway);
        break;
    case 104:
        unsigned char buf[4];
	    buf[0] = (unsigned char) ipAlarmElement.majorUnder;
	    buf[1] = (unsigned char) ipAlarmElement.minorUnder;
	    buf[2] = (unsigned char) ipAlarmElement.minorOver;
	    buf[3] = (unsigned char) ipAlarmElement.majorOver;
	    lcdTempData.setTemp(buf);
        break;
    }
}

void proc_KeyState(char key, char mode) {
    switch(mode) {
    case 101:
    case 102:
    case 103:
        lcdTupleData.procKey(key);
        break;
    case 104:
        lcdTempData.procKey(key);
        break;
    }
}

void proc_AcceptState(char mode){
    switch(mode){
    case 101:
        lcdTupleData.getTuple(ipAlarmElement.ip);
        a_assignIP(ipAlarmElement.ip);
        saveIPAlarm();
        break;
    case 102:
        lcdTupleData.getTuple(ipAlarmElement.mask);
        a_assignMask(ipAlarmElement.mask);
        saveIPAlarm();
        break;
    case 103:
        lcdTupleData.getTuple(ipAlarmElement.gateway);
        a_assignGateway(ipAlarmElement.gateway);
        saveIPAlarm();
        break;
    case 104:
        unsigned char buf[4];
	    lcdTempData.getTemp(buf);
        ipAlarmElement.majorUnder = buf[0];
        ipAlarmElement.minorUnder = buf[1];
        ipAlarmElement.minorOver = buf[2];
        ipAlarmElement.majorOver = buf[3];
        saveIPAlarm();
        break;
    }
}