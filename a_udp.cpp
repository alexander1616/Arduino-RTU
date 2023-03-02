#include <Arduino.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include "a_alarmStates.h"

typedef struct {
    byte mac [6];
    unsigned int localPort;
    char packetBuf[80];
    EthernetUDP Udp;
    char hardWareCheck;
    char udpLinkCheck;
    char udpReady;
} UdpData_t;

UdpData_t UdpData = {
    {0x90, 0xA2, 0xDA, 0x0D, 0x85, 0xD9},
    8888, {0}, {}, -1, -2, -1
};

IPAddress ip(192, 168, 20, 10);
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
              Serial.println(F("cable"));
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
      Serial.println(F("cable"));
      outTime = millis();
      UdpData.udpLinkCheck = -2;
      UdpData.udpReady = -1;
  } else {
      UdpData.udpLinkCheck = 0;
  }
  alarmNetwork(UdpData.udpLinkCheck);
  return UdpData.udpReady;
}
  
void a_udpSetup(){
  // start the Ethernet
  Ethernet.begin(UdpData.mac, ip);
  a_udpCheckStatus();
  UdpData.Udp.begin(UdpData.localPort);
}

char* a_udpPacketReader() {
    if (a_udpCheckStatus()){
        return 0;
    }
    int packetSize = UdpData.Udp.parsePacket();
    if (packetSize) {
        Serial.print("Received ");
        Serial.println(packetSize);
        Serial.print("From ");
        IPAddress remote = UdpData.Udp.remoteIP();
        for (int i=0; i < 4; i++) {
            Serial.print(remote[i], DEC);
            if (i < 3) {
                Serial.print(".");
            }
        }
        Serial.print(", port ");
        Serial.println(UdpData.Udp.remotePort());

        // read the packet into packetBuffer
        // if sizeof(UdpData.packetBuf) < packetSize , then err
        int n;
        n = UdpData.Udp.read(UdpData.packetBuf, sizeof(UdpData.packetBuf));
        Serial.println(n);
        UdpData.packetBuf[n] = 0;
        Serial.println("Contents:");
        Serial.println(UdpData.packetBuf);
        return UdpData.packetBuf;
    }
    return 0;
}
void a_udpSendAlert(char* msg){
    Serial.print("UDP ");
    Serial.println(msg);
    // send a reply to the IP address and port that sent us the packet we received
    UdpData.Udp.beginPacket(UdpData.Udp.remoteIP(), UdpData.Udp.remotePort());
    UdpData.Udp.write(msg);
    UdpData.Udp.endPacket();
}
