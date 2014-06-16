// BatLib.h

#ifndef _BATLIB_h
#define _BATLIB_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include <EEPROM.h>
#include <SdFat.h>

const int PIN_LED_GREEN = 5;
const int PIN_LED_YELLOW = 6;
const int PIN_LED_RED = 7;

const int PIN_SD_PWR = 3;
const int PIN_SD_CS = 10;

const int PIN_BAT_IN = 2;

const int PIN_S1 = 8;
const int PIN_S2 = 9;

class BatBoardClass
{
	public:
		BatBoardClass();
		void init();

		// Utilities
		long getVcc();
		
		// Helpers	
		void setLeds(int green, int yellow, int red);
		void error(int code);
	private:
		long getVcc(long refMiliVoltage);

		//Helpers
		long getRefVoltageSetting();
};


extern BatBoardClass BatBoard;

template <class T> int EEPROM_read(int adress, T& value)
{
	byte* p = (byte*)(void*)&value;
	unsigned int i;
	for (i = 0; i < sizeof(value); i++)
		*p++ = EEPROM.read(adress++);
	return i;
}

#endif

