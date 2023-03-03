#ifndef a_udp_h
#define a_udp_h 1

extern void a_udpSetup();
extern char a_udpCheckStatus();
extern void a_udpSendAlert(char*);
extern char* a_udpPacketReader();
extern void a_udpSendBroadcast(char*);

#endif