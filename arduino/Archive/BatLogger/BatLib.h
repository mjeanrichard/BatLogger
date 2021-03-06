// BatLib.h

#ifndef _BATLIB_h
#define _BATLIB_h

#include "Arduino.h"

#define BATLOGGER_VERSION 01

#include <EEPROM.h>
#include <SdFat.h>

#if BATLOGGER_VERSION == 1
	const int PIN_LED_GREEN = 3;
	const int PIN_LED_YELLOW = 6;
	const int PIN_LED_RED = 9;
#endif

const int PIN_BAT_IN = 2;

const int PIN_S1 = 8;
const int PIN_S2 = 2;

class BatBoardClass
{
	public:
		BatBoardClass();
		void init();
		
		void initCounter();
		unsigned long readCounter();

		// Utilities
		long getVcc();
		
		// Helpers	
	private:
		long getVcc(long refMiliVoltage);

		//Helpers
		long getRefVoltageSetting();
};

void BatLib_SetLeds(bool green, bool yellow, bool red);
void BatLib_Error(int code);

template <class T> int EEPROM_read(int adress, T& value)
{
	byte* p = (byte*)(void*)&value;
	unsigned int i;
	for (i = 0; i < sizeof(value); i++)
		*p++ = EEPROM.read(adress++);
	return i;
}

#endif

