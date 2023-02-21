#include <Arduino.h>
#include "readLine.h"

void setup(){
	Serial.begin(9600);
}

void loop(){
	char *p;
	while (!Serial.available()){;};
	p = readLine();
	Serial.println(p);
}
