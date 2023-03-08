#include <Arduino.h>
#include "a_analogButton.h"
#include "a_lcdSetting.h"
#include "a_lcd.h"

/*
*   2 LINE LCD
*
*   IP  Msk Gat Tem  -> 4 bytes tuple
*           ^^^      -> current marker 0-3
*                         up key will increase the number
*                         dn key will decrease the number
*
*/

lcdSetting lcdSettingData;

#define KEY_LEFT 1
#define KEY_RIGHT 4
#define KEY_UP 2
#define KEY_DOWN 3
#define KEY_ENTER 5
#define KEY_REFRESH 0


void lcdSetting::procKey(char key) {
	if (key == KEY_UP) {
		byteBuf[cur] = byteBuf[cur] + 1; 
	} else if (key == KEY_DOWN) {
		byteBuf[cur] = byteBuf[cur] - 1; 
	} else if (key == KEY_LEFT) {
		cur--;
		if (cur < 0) {
			cur = 3;
		}
	} else if (key == KEY_RIGHT) {
		cur++;
		if (cur > 3) {
			cur = 0;
		}
	} else if (key == KEY_REFRESH) {
		drawLine0();
		drawLine1();
	}
}

void lcdSetting::setSetting(char mode) {
	cur = mode;	
}

char lcdSetting::getSetting() {
	return cur;
}

//
//	Drawing the lcd here
//
void lcdSetting::drawLine0() {
	a_lcdPrint(0, "I.P MSK G.W TEM");
}

void lcdSetting::drawLine1() {
	char buf[17];
	char *cp = buf;
	for (char i = 0; i < 4; i++) {
		if ( i == cur ) {
			*cp++ = '^';
			*cp++ = '^';
			*cp++ = '^';
		} else {
			*cp++ = ' ';
			*cp++ = ' ';
			*cp++ = ' ';
		}
		*cp++ = ' ';
	}
	buf[15] = 0;
	a_lcdPrint(1, buf);
}
