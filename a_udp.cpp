#include <Arduino.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>
#include "a_alarmStates.h"
#include "a_lcd.h"

/*************************************************
*           Functionality with UDP               *
*                                                *
*    Reads incoming packet from UDP              *
*    Processes data into a string buffer         *
*************************************************/

typedef struct {
    byte mac [6];
    unsigned int localPort;
    char packetBuf[80];
    EthernetUDP Udp;
    char hardWareCheck;
    char udpLinkCheck;
    char udpReady;
    unsigned char ip[4];
    unsigned char gateway[4];
    unsigned char mask[4];
    unsigned int prcv;
    unsigned int ptx;
} UdpData_t;

UdpData_t UdpData = {
    {0x90, 0xA2, 0xDA, 0x0D, 0x85, 0xD9},
    8888, {0}, {}, -1, -2, -1,
    {192, 168, 20, 10},
    {192, 168, 20, 1},
    {255, 255, 255, 0},
    0, 0
};

// IPAddress mask(255, 255, 255, 0);
// IPAddress gateway(192, 168, 20, 1);
// IPAddress ip(192, 168, 20, 10);
static unsigned long outTime = 0;

char a_udpCheckStatus() {
  // Check for Ethernet hardware present
  UdpData.udpReady = 0;
  // if (Ethernet.hardwareStatus() == EthernetNoHardware) {
  //   Serial.println(F("board"));
  //   UdpData.hardWareCheck = -1;
  //   UdpData.udpReady = -1;
  //   //alarmNetwork(UdpData.hardWareCheck);
  // } else {
  //   UdpData.hardWareCheck = 0;
  // }
  if (UdpData.udpLinkCheck){
      unsigned long timecurrent;
      timecurrent = millis();
      unsigned long tdelay;
      tdelay = timecurrent - outTime;
      if (tdelay >= 10000){
          if (Ethernet.linkStatus() == LinkOFF) {
              Serial.println(F("25"));
              outTime = millis();
              UdpData.udpLinkCheck = -2;
              UdpData.udpReady = -1;
          } else {
              UdpData.udpLinkCheck = 0;
          }
      } else {
        UdpData.udpReady = -1;
      }
  } else if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println(F("25"));
      outTime = millis();
      UdpData.udpLinkCheck = -2;
      UdpData.udpReady = -1;
  } else {
      UdpData.udpLinkCheck = 0;
  }
  alarmNetwork(UdpData.udpLinkCheck);
  return UdpData.udpReady;
}

//change UDP must call
void a_udpSetup(){
  // start the Ethernet
  //Serial.println("xxxxxxin udpSetup ");
  IPAddress ip(UdpData.ip[0], UdpData.ip[1], UdpData.ip[2], UdpData.ip[3]);
  Serial.println(F("SETUP: "));
  Serial.println(ip);
  Serial.println(UdpData.localPort);
  Ethernet.begin(UdpData.mac, ip);
  a_udpCheckStatus();
  UdpData.Udp.begin(UdpData.localPort);
}

void a_assignIP(unsigned char ip[4]){
    //Serial.println("in assignIp");
    for (int i = 0; i < 4; i++){
        UdpData.ip[i] = ip[i];
        // Serial.print(UdpData.ip[i]);
    }
    // Serial.println("going into udpSetup");
    a_udpSetup();
    //Ethernet.setLocalIP
}

void a_assignGateway(unsigned char ip[4]){
    for (int i = 0; i < 4; i++){
        UdpData.gateway[i] = ip[i];
    }
    //Ethernet.setGatewayIP(UpdateGateway)
}

void a_assignMask(unsigned char ip[4]){
    for (int i = 0; i < 4; i++){
        UdpData.mask[i] = ip[i];
    }
    //Ethernet.setSubnetMask
}

char* a_udpPacketReader() {
    if (a_udpCheckStatus()){
        return 0;
    }
    int packetSize = UdpData.Udp.parsePacket();
    if (packetSize) {
        if(packetSize == 0x4000){
            Serial.println(F("UDP ERR"));
            return 0;
        }
        Serial.print(F("Rcv "));
        Serial.println(packetSize);
        Serial.print(F(" "));
        IPAddress remote = UdpData.Udp.remoteIP();
        for (int i=0; i < 4; i++) {
            Serial.print(remote[i], DEC);
            if (i < 3) {
                Serial.print(F("."));
            }
        }
        Serial.print(F(", p "));
        Serial.println(UdpData.Udp.remotePort());

        // read the packet into packetBuffer
        // if sizeof(UdpData.packetBuf) < packetSize , then err
        int n;
        n = UdpData.Udp.read(UdpData.packetBuf, sizeof(UdpData.packetBuf)-1);
        Serial.println(n);
        UdpData.packetBuf[n] = 0;
        Serial.println(F("Buf"));
        Serial.println(UdpData.packetBuf);
        UdpData.prcv ++;
        return UdpData.packetBuf;
    }
    return 0;
}

void a_udpSendBroadcast(char* msg){
    if (UdpData.udpReady){
      return;
    }
    Serial.print(F("UDP "));
    Serial.println(msg);
    // send a reply to the IP address and port that sent us the packet we received
    //IPAddress broacast(255,255,255,255);
    UdpData.Udp.beginPacket(IPAddress(255,255,255,255), 8888);
    UdpData.Udp.write(msg);
    UdpData.Udp.endPacket();
    UdpData.ptx ++;
}

void a_udpSendAlert(char* msg){
    if (UdpData.udpReady){
      return;
    }
    Serial.print(F("UDP "));
    Serial.println(msg);
    // send a reply to the IP address and port that sent us the packet we received
    UdpData.Udp.beginPacket(UdpData.Udp.remoteIP(), UdpData.Udp.remotePort());
    UdpData.Udp.write(msg);
    UdpData.Udp.endPacket();
    UdpData.ptx ++;
}

void a_lcdPrintPackets(){
    char line1buf[17];
    char line2buf[17];
    snprintf(line1buf, 17, "Rcv: %d", UdpData.prcv);
    snprintf(line2buf, 17, "Tx: %d", UdpData.ptx);
    a_lcdPrint(0, line2buf);
    a_lcdPrint(1, line1buf);
}