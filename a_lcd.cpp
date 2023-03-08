#include <Arduino.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "a_analogButton.h"
#include "a_dht.h"
#include "a_udp.h"
#include "a_lcdSetting.h"

#define KEY_LEFT 1
#define KEY_RIGHT 4
#define KEY_UP 2
#define KEY_DOWN 3
#define KEY_ENTER 5

char c_state = 0;
char c_substate = 100; //100 main menu, ip 101, mask 102, gate 103, temp 104
unsigned char c_stateChange = 0;
char c_hscroll = 0;
//int c_move = 0;
//extern char dht_dataFlag;

LiquidCrystal_I2C lcd(0x27,16,2);

void a_lcdInit(){
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("Hello!");
    // lcdShowTemp();
    // lcdShowHistory();
}

void a_lcdPrint(char row, char*str){
    lcd.setCursor(0, row);
    char c;
    char buf[17];
    int i;
    for (i = 0; i<16; i++){
        c = *str++;
        if (c == 0){
            for(; i<16; i++){
                buf[i] = ' ';
            }
        } else {
            buf[i] = c;
        }
    }
    buf[16] = 0;
    lcd.print(buf);
}

void a_lcdPrint0(char *str){
    a_lcdPrint(0, str);
}

void a_lcdPrint1(char* str){
    a_lcdPrint(1, str);
}

void a_lcdHome(){
    lcdShowTemp();
}

void a_lcdHistory(){
    lcdShowHistory(0);
}

void a_lcdStats(){
    a_lcdPrintPackets();
}

void a_lcdSettings(){
    lcdSettingData.setSetting(0);
}

void a_lcdReset(){
    a_lcdPrint(0, "Erase History");
    a_lcdPrint(1, "Enter | Leave");
}

void (*c_statefunc)();

// char a_lcdCheckButton(){
    // analogKey xKey;
    // char keyPressed = xKey.getKey();
    // switch (c_state){
    // case 0:
    //     if (keyPressed == KEY_LEFT){
    //         c_state = 3;
    //         //c_statefunc = a_lcdSettings;
    //     } else if (keyPressed == KEY_RIGHT){
    //         c_state = 1;
    //         //c_statefunc = a_lcdHistory;
    //     } 
    //     break;
    // case 1:
    //     // print history screen
    //     if (keyPressed == KEY_LEFT){
    //         c_state = 0;
    //         //c_statefunc = a_lcdHome;
    //     } else if (keyPressed == KEY_RIGHT){
    //         c_state = 2;
    //         //c_statefunc = a_lcdStats;
    //     } 
    //     break;
    // case 2:
    //     // print stat screen
    //     if (keyPressed == KEY_LEFT){
    //         c_state = 1;
    //         //c_statefunc = a_lcdHistory;
    //     } else if (keyPressed == KEY_RIGHT){
    //         c_state = 3;
    //         //c_statefunc = a_lcdSettings;
    //     } 
    //     break;
    // case 3:
    //     // print settings screen
    //     if (keyPressed == KEY_LEFT){
    //         c_state = 2;
    //         //c_statefunc = a_lcdStats;
    //     } else if (keyPressed == KEY_RIGHT){
    //         c_state = 0;
    //         //c_statefunc = a_lcdHome;
    //     } else if (keyPressed == KEY_ENTER){
    //         //configureMode();
    //         ;
    //     } 
    //     break;
    // default:
    //     break;
    // }
    // return keyPressed;
// }

void checkScreenRefresh(){
    static unsigned long refreshtime = 0;
    unsigned long timecurrent;
    timecurrent = millis();
    unsigned long tdelay;
    tdelay = timecurrent - refreshtime;
    if (tdelay >= 1000){   
        if(c_state == 0){
             a_lcdHome();
        } else if (c_state == 2){
            a_lcdStats();
        } else if (c_state == 3){
            if (c_substate == 100){
                lcdSettingData.procKey(0);
            } else if ((c_substate >= 101)&&(c_substate <= 104)){
                proc_KeyState(0, c_substate);
            }
        }
        refreshtime = timecurrent;
    }
}

void a_settingProcess(char key){
    if (c_substate == 100){
        if (key == KEY_SW2){
            c_state = 2;
            c_stateChange = 1;
        } else if (key == KEY_SW3){
            c_state = 4;
            c_stateChange = 1;
        } else if (key == KEY_SW5){
            int n;
            n = lcdSettingData.getSetting();
            c_substate = 101+n;
            proc_initKeyState(c_substate);
        } else {
            lcdSettingData.procKey(key);
        }
        return;
    }
    if (key == KEY_SW5){
        proc_AcceptState(c_substate);
        c_substate = 100;
    } else {
        proc_KeyState(key, c_substate);
    }
}

char a_lcdStateChange(){
    char key;
    analogKey button;
    key = button.getKey();
    if (c_state == 3){
        if (key == KEY_SW0){
            checkScreenRefresh();
            return 0;
        }
        a_settingProcess(key);
        return 0;
    }
    switch (key){
    case KEY_SW0:
        checkScreenRefresh();
        break;
    case KEY_SW1:
        //Serial.println("[1] Pressed");
        c_state--;
        c_stateChange = 1;
        break;
    case KEY_SW2:
        //Serial.println("[2] Pressed");
        c_hscroll = 1;
        //c_move--;
        break;
    case KEY_SW3:
        //Serial.println("[3] Pressed");
        c_hscroll = 1;
        //c_move++;
        break;
    case KEY_SW4:
        //Serial.println("[4] Pressed");
        c_state++;
        c_stateChange = 1;
        break;
    case KEY_SW5:
        if (c_state == 4){
            recordInit();
            c_state = 0;
            c_stateChange = 1;
        }
        //Serial.println("[5] Pressed");
        break;
    case KEY_SWX:
        //Serial.println("Unknown key");
        break;
    default:
        //Serial.println("Impossible!");
        break;
    }
    if(c_state > 4){
        c_state = 0;
    }
    if(c_state < 0){
        c_state = 4;
    }
    return key;
}

unsigned long refreshtime = 0;
void a_lcdLoopMenu() {
    char key = 0;
    key = a_lcdStateChange();
    if (c_stateChange){
        c_stateChange = 0;
        switch(c_state){
        case 0:
            a_lcdHome();
            break;
        case 1:
            //test_lcdShowHistory(c_move);
            lcdShowHistory(0);
            break;
        case 2:
            a_lcdStats();
            break;
        case 3:
            a_lcdSettings();
            break;
        case 4:
            a_lcdReset();
            break;
        default:
            c_state = 0;
            a_lcdHome();
            break;
        }
    }
    if (c_hscroll){
        c_hscroll = 0;
        if (c_state == 1){
            if (key == KEY_SW2){
                lcdShowHistory(-1);
            } else if (key == KEY_SW3){
                lcdShowHistory(1);
            }
        }
    }
}