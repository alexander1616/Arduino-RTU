#include <Arduino.h>
#include "parseInput.h"
#define MAX_TOKEN_SIZE 12
#include "projectDef.h"
#define MAXCMDBUF 30

/*************************************************
*               Parse Input                      *
*                                                *
*    Parses a string, looks for "commands"       *
*    to store in command buffer to be executed   *
*    Commands can be configured in defines       *
*************************************************/


//character array to help determine system state
char stateTable[256] = {
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3,
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

/*
action = 0 (initial)
action = 1 (alpha -> append)
action = 2 (only digits -> append, set num)
action = 8 (error, bad token)
action = 9 (got token)
*/
char actionMap[5][5] = {
    {8, 1, 2, 0, 7},
    {8, 1, 1, 9, 8},
    {8, 8, 2, 9, 8},
    {8, 8, 8, 8, 8},
    {8, 8, 2, 8, 8}
};

//token element helper functions

typedef struct {
    int len;
    char buf[MAX_TOKEN_SIZE +1];
    int num;
} tokenElement_t;

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
    tokenAddChar(c);
};


//command buf element and helper functions
typedef struct {
    unsigned char buf[30];
    char count;
} commandBuf_t;

commandBuf_t cmdBuf = {
    {t_EOL}, 0
};

void addCmdBuf(unsigned char c){
    cmdBuf.buf[cmdBuf.count] = c;
    cmdBuf.count++;
    if (cmdBuf.count >= MAXCMDBUF){
        Serial.println(F("Error, too many commands"));
    }
};

void newCmdBuf(){
  int i = 0;
  for (;i<MAXCMDBUF;i++){
    cmdBuf.buf[i] = t_EOL;
  }
  cmdBuf.count = 0;
};


//lookup table structure, command element, lookup function

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
    {'r', 'g', 2, t_RG},
    {'c', 'l', 5, t_CLOCK},
    {'a', 'd', 3, t_ADD},
    {'t', 'e', 4, t_TEMP},
    {'h', 'i', 7, t_HISTORY},
    {'e', 'e', 6, t_EEPROM},
    {'m', 'a', 3, t_MAX},
    {'m', 'i', 3, t_MIN},
    {'r', 'g', 3, t_RGB}
};

#define numCommand sizeof(lookUpTable)/sizeof(lookUpTable[0])

unsigned char lookUpToken(char* s, int len){
    int i;
    char c = isUpperCase(s[0])?toLowerCase(s[0]):s[0];
    char c1 = isUpperCase(s[1])?toLowerCase(s[1]):s[1];
    char buf[50];
    for (i = 0; i < numCommand; i++){
        if ((lookUpTable[i].key[0] == c) && 
            (lookUpTable[i].key[1] == c1) && 
            (lookUpTable[i].len == len)){
                // snprintf(buf, 50, "found:[%s][%i]", s, lookUpTable[i].cmd);
                // Serial.println(buf);
                return lookUpTable[i].cmd;
            }
    }
    snprintf(buf, 50, "Bad Command:[%s]", s);
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
    //bsProcess(s);
    newCmdBuf();
    newToken();
    int currentstate = 0;
    int previousstate = 0;
    char cur;
    int action;
    while(cur = *s++){
        currentstate = stateTable[cur];
        action = actionMap[previousstate][currentstate];
        // snprintf(buf, 40, "CurPreChar[%d][%d][%c]", currentstate, previousstate, cur);
        // Serial.println(buf);
        switch (action) {
        case 1:
            tokenAddChar(cur);
            if (previousstate == 1){
                currentstate = 1;
            };
            break;
        case 2:
            tokenAddNum(cur);
            break;
        case 7:
            addCmdBuf(t_NEG);
            break;
        case 8:
            // Serial.println("Bad Input");
            // Serial.println(cur);
            return 0;
            break;
        case 9:
            if (previousstate == 2){
                addCmdBuf(t_WORD);
                addCmdBuf((unsigned char)(tokenState.num>>8));
                addCmdBuf((unsigned char)tokenState.num);
                // for (int i = 0; i < cmdBuf.count; i++){
                //     Serial.print("Cmd buf added num: ");
                //     Serial.println(cmdBuf.buf[i]);
                // }
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
            currentstate = 0;
            break;
        };
        previousstate = currentstate;
    };

    //Process leftover
    if (tokenState.len != 0){
      if (previousstate == 2){
        addCmdBuf(t_WORD);
        addCmdBuf((unsigned char)(tokenState.num>>8));
        addCmdBuf((unsigned char)tokenState.num);
        // for (int i = 0; i < cmdBuf.count; i++){
        //     Serial.print("Last ONE! Cmd buf added num: ");
        //     Serial.println(cmdBuf.buf[i]);
        // }
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