#ifndef a_udp_h
#define a_udp_h 1

extern void a_udpSetup();
extern char a_udpCheckStatus();
extern void a_udpSendAlert(char*);
extern char* a_udpPacketReader();
extern void a_udpSendBroadcast(char*);
extern void a_assignIP(unsigned char ip[4]);
extern void a_assignGateway(unsigned char gateway[4]);
extern void a_assignMask(unsigned char mask[4]);
extern void a_lcdPrintPackets();

#endif