
#include <Arduino.h>
#include "a_analogButton.h"
#include "a_lcdTemp.h"
#include "a_lcd.h"

/*
*   2 LINE LCD
*
*   000 000 000 000       -> 4 bytes tuple
*           ^^^           -> current marker 
*                         up key will increase the number
*                         dn key will decrease the number
*  setTemp -> initial tuple setting of the number
*  getTemp -> set the tuple to dst place
*
*/

lcdTemp lcdTempData;

#define KEY_LEFT 1
#define KEY_RIGHT 4
#define KEY_UP 2
#define KEY_DOWN 3
#define KEY_ENTER 5
#define KEY_REFRESH 0


void lcdTemp::procKey(char key) {
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

void lcdTemp::setTemp(unsigned char * src_p) {
	for (int i = 0; i < 4; i++) {
		byteBuf[i] = *src_p++;
	}
	cur = 0;	//
}

void lcdTemp::getTemp(unsigned char * dst_p) {
	for (int i = 0; i < 4; i++) {
		*dst_p++ = byteBuf[i];
	}
}

//
//	Drawing the lcd here
//
void lcdTemp::drawLine0() {
	char buf[17];
	snprintf(buf, 17, "%3d %3d %3d %3d", 
			byteBuf[0], byteBuf[1], byteBuf[2], byteBuf[3]);
	buf[15] = 0;
	a_lcdPrint(0, buf);
}

void lcdTemp::drawLine1() {
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
