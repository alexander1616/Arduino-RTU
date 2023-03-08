#include <Arduino.h>
#include "a_analogButton.h"
#include "a_lcdTuple.h"
#include "a_lcd.h"

/*************************************************
*            Tuple Setting Display               *
*                                                *
*    Displays main page for configuring tuples   *
*    Functionality: Select, Enter, LCD display   *
*************************************************/

/*
*   2 LINE LCD
*
*   192.168. 10. 23  -> 4 bytes tuple
*           ^^^      -> current marker 0-3
*                         up key will increase the number
*                         dn key will decrease the number
*  setTuple -> initial tuple setting of the number
*  getTuple -> set the tuple to dst place
*
*/

lcdTuple lcdTupleData;

#define KEY_LEFT 1
#define KEY_RIGHT 4
#define KEY_UP 2
#define KEY_DOWN 3
#define KEY_ENTER 5
#define KEY_REFRESH 0


void lcdTuple::procKey(char key) {
	if (key == KEY_UP) {
		byteBuf[cur] = byteBuf[cur] + 1; 
		drawLine0();
	} else if (key == KEY_DOWN) {
		byteBuf[cur] = byteBuf[cur] - 1; 
		drawLine0();

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

void lcdTuple::setTuple(unsigned char * src_p) {
	for (int i = 0; i < 4; i++) {
		byteBuf[i] = *src_p++;
	}
	cur = 3;	// usually start from the last tuple
}

void lcdTuple::getTuple(unsigned char * dst_p) {
	for (int i = 0; i < 4; i++) {
		*dst_p++ = byteBuf[i];
	}
}

//
//	Drawing the lcd here
//
void lcdTuple::drawLine0() {
	char buf[17];
	snprintf(buf, 17, "%3d.%3d.%3d.%3d", 
			byteBuf[0], byteBuf[1], byteBuf[2], byteBuf[3]);
	buf[15] = 0;
	a_lcdPrint(0, buf);
}

void lcdTuple::drawLine1() {
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