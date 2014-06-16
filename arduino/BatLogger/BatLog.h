// LogClass.h

#ifndef _LOGCLASS_h
#define _LOGCLASS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "SdFat.h"

const int PIN_SD_PWR = 3;
const int PIN_SD_CS = 10;


const int ERR_POWER_FAIL = 1;

const int ERR_SD_BEGIN = 2;
const int ERR_SD_OPEN = 3;
const int ERR_SD_CREATE = 4;

class BatLog
{
 private:
	SdFat _sd;
	SdFile _logFile;

	int _indicatorLedPin;
	char _filename[9];

	void openLogFile();
 public:
	void init(int indicatorLed);
	void start();
	void stop();

	int	printf(const char * fromatString, ...);
};

#endif

