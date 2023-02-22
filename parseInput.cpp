#include <Arduino.h>
#include "parseInput.h"
#define MAX_TOKEN_SIZE 12
#include "projectDef.h"

char stateTable[256] = {
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3,
    3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 3,
    3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3
};

typedef struct {
    int len;
    char buf[MAX_TOKEN_SIZE +1];
    int num;
} tokenElement_t;

/*
action = 0 (initial)
action = 1 (alpha -> append)
action = 2 (only digits -> append, set num)
action = 8 (error, bad token)
action = 9 (got token)
*/
char actionMap[4][4] = {
    {8, 1, 2, 8},
    {8, 1, 1, 9},
    {8, 8, 2, 9},
    {8, 8, 8, 8}
};

tokenElement_t tokenState = {
    0, {0}, 0
};

void newToken(){
    tokenState.len = 0;
    tokenState.num = 0;
};

void tokenAddChar(char c){
    tokenState.buf[tokenState.len] = c;
    tokenState.len++;
};

void tokenAddNum(char c){
    int val = 0;
    val = c - '0';
    tokenState.num = tokenState.num*10 + val;
};

typedef struct {
    unsigned char buf[10];
    char count;
} commandBuf_t;

commandBuf_t cmdBuf = {
    {t_EOL}, 0
};

void addCmdBuf(unsigned char c){
    cmdBuf.buf[cmdBuf.count] = c;
    cmdBuf.count++;
};

void newCmdBuf(){
  int i = 0;
  for (;i<10;i++){
    cmdBuf.buf[i] = t_EOL;
  }
  cmdBuf.count = 0;
};

typedef struct {
    unsigned char key[2];
    unsigned char len;
    unsigned char cmd;
} cmdElement_t;

cmdElement_t lookUpTable[] = {
    {'o', 'f', 3, t_OFF},
    {'o', 'n', 2, t_ON},
    {'l', 'e', 3, t_LED},
    {'r', 'e', 3, t_RED},
    {'g', 'r', 5, t_GREEN},
    {'d', '1', 3, t_D13},
    {'s', 'e', 3, t_SET},
    {'b', 'l', 5, t_BLINK},
    {'l', 'e', 4, t_LEDS},
    {'s', 't', 6, t_STATUS},
    {'h', 'e', 4, t_HELP},
    {'v', 'e', 7, t_VERSION},
    {'r', 'g', 2, t_RG}
};

#define numCommand sizeof(lookUpTable)/sizeof(lookUpTable[0])

unsigned char lookUpToken(char* s, int len){
    int i;
    char c = isUpperCase(s[0])?toLowerCase(s[0]):s[0];
    char c1 = isUpperCase(s[1])?toLowerCase(s[1]):s[1];
//    Serial.print("Looking up token: c: ");
//    Serial.print(c);
//    Serial.print(" c1: ");
//    Serial.print(c1);
//    Serial.print(" len: ");
//    Serial.print(len);
//    Serial.print(" s: ");
//    Serial.println(s);
    char buf[50];
    for (i = 0; i < numCommand; i++){
//        Serial.print("LookupTable 0, 1, len ");
//        Serial.print(lookUpTable[i].key[0]);
//        Serial.print(" ");
//        Serial.print(lookUpTable[i].key[1]);
//        Serial.print(" ");
//        Serial.println(lookUpTable[i].len);
        if ((lookUpTable[i].key[0] == c) && 
            (lookUpTable[i].key[1] == c1) && 
            (lookUpTable[i].len == len)){
                snprintf(buf, 50, "found:[%s][%i]", s, lookUpTable[i].cmd);
                Serial.println(buf);
                return lookUpTable[i].cmd;
            }
    }
    snprintf(buf, 50, "Not found:[%s]", s);
    Serial.println(buf);
    return t_UNKNOWN;
};

/*This function processes backspace and delete*/
void bsProcess(char* s){
    char c;
    int cur;
    int eff;
    eff = cur = 0;
    for (;c = s[cur]; cur++){
        if ((c == 0x08)||(c == 0x7F)){
            if (eff > 0){
                eff--;
            }
        } else {
            if (eff != cur){
                s[eff] = c;
            }
            eff++;
        }
    }
    s[eff] = 0;
}

unsigned char* parseInput(char* s){
    char buf[40];
    //snprintf(buf, 40, "+[%s]", s);
    //Serial.println(buf);
    bsProcess(s);
    //snprintf(buf, 40, "after[%s]", s);
    //Serial.println(buf);
    newCmdBuf();
    newToken();
    int currentstate = 0;
    int previousstate = 0;
    char cur;
    int action;
    //while(1){
    //    cur = *s++;
    while(cur = *s++){
        currentstate = stateTable[cur];
        action = actionMap[previousstate][currentstate];
        snprintf(buf, 40, "CurPreChar[%d][%d][%c]", currentstate, previousstate, cur);
        Serial.println(buf);
        // Serial.print("Printing current state: ");
        // Serial.println(currentstate);
        // Serial.print("Printing previous state: ");
        // Serial.println(previousstate);
        // Serial.print("Character is: ");
        // Serial.println(cur);
        switch (action) {
        case 1:
            tokenAddChar(cur);
            if (previousstate == 1){
                Serial.println("assign curentstate to 1");
                currentstate = 1;
            };
            break;
        case 2:
            tokenAddNum(cur);
            break;
        case 8:
            Serial.println("Bad Input");
            Serial.println(cur);
            return 0;
            break;
        case 9:
            if (previousstate == 2){
                addCmdBuf(t_WORD);
                addCmdBuf((unsigned char)(tokenState.num>>8));
                addCmdBuf((unsigned char)tokenState.num);
                for (int i = 0; i < cmdBuf.count; i++){
                    Serial.print("Cmd buf added num: ");
                    Serial.println(cmdBuf.buf[i]);
                }
            } else {
                unsigned char cmd;
                tokenAddChar(0);
                cmd = lookUpToken(tokenState.buf, tokenState.len - 1);
                if (cmd == t_UNKNOWN){
                    return 0;
                }
                addCmdBuf(cmd);
            };
            newToken();
            currentstate = 0;
            break;
        default:
            break;
        };
        previousstate = currentstate;
    };

    //Process leftover, need updating
    if (tokenState.len != 0){
      if (previousstate == 2){
        addCmdBuf(t_WORD);
        addCmdBuf((unsigned char)(tokenState.num>>8));
        addCmdBuf((unsigned char)tokenState.num);
        for (int i = 0; i < cmdBuf.count; i++){
            Serial.print("Cmd buf added num: ");
            Serial.println(cmdBuf.buf[i]);
        }
      } else {
        unsigned char cmd;
        tokenAddChar(0);
        cmd = lookUpToken(tokenState.buf, tokenState.len - 1);
        if (cmd == t_UNKNOWN){
          return 0;
        }
        addCmdBuf(cmd);
      };
    }

    addCmdBuf(t_EOL);
    return cmdBuf.buf;
};
