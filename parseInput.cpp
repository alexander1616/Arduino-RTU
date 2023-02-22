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
    {'v', 'e', 7, t_VERSION}
};

unsigned char* parseInput(char* s){
    int currentstate = 0;
    int previousstate = 0;
    char cur;
    int action;

    while(cur = *s++){
        currentstate = stateTable[cur];
        action = actionMap[previousstate][currentstate];
        Serial.println(action);
    }
    return cmdBuf;
}