#ifndef a_lcdSetting_h
#define a_lcdSetting_h 1

/*
*   2 LINE LCD
*
*   192.168. 10. 23  -> 4 bytes tuple
*           ---      -> current marker 0-3
*                         up key will increase the number
*                         dn key will decrease the number
*  setTuple -> initial tuple setting of the number
*  getTuple -> set the tuple to dst place
*
*/

class lcdSetting{
public:
	void procKey(char key);
	void setSetting(char mode);
	char getSetting();

private:
	void drawLine0();
	void drawLine1();

private:
	unsigned char byteBuf[4];
	char cur;		// where the current setting
};

// 
extern lcdSetting lcdSettingData;
#endif