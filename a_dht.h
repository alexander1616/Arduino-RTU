#ifndef a_dht_h
#define a_dht_h 1

extern void dhtShowHistory();
extern void dhtShowTemp();
extern void dhtLoop();
extern void dhtSetShowTemp(int);
extern void dhtDataInit();
extern void dhtShowEEProm();
extern void dhtEEPromInit();
extern void dhtShowMaxTemp();
extern void dhtShowMinTemp();
extern void lcdShowTemp();
extern void lcdShowHistory(char mode); //mode = 0, last, 1
extern void test_lcdShowHistory(int move);
extern void proc_initKeyState(char mode);
extern void proc_KeyState(char key, char mode);
extern void proc_AcceptState(char mode);
extern void recordInit();

#endif