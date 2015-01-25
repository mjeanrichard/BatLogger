#include "BatLog.h"
#include "BatLib.h"

void BatLog::init(int indicatorLed)
{
	pinMode(PIN_SD_PWR, OUTPUT);

	strcpy_P(_filename, PSTR("LOG--.CSV"));
	_indicatorLedPin = indicatorLed;
}

void BatLog::start()
{
	if (_indicatorLedPin >= 0){
		digitalWrite(_indicatorLedPin, HIGH);
	}
	digitalWrite(PIN_SD_PWR, HIGH);
	delay(200);

	if (!_sd.begin(PIN_SD_CS, SPI_FULL_SPEED)){
		BatLib_Error(ERR_SD_BEGIN);
	}

	if (!_sd.exists(_filename)) {
		for (int i = 0; i < 100; i++) {
			_filename[3] = i / 10 + '0';
			_filename[4] = i % 10 + '0';
			if (!_sd.exists(_filename)) {
				if (!_logFile.open(_filename, O_RDWR | O_CREAT | O_AT_END)) {
					BatLib_Error(ERR_SD_CREATE);
				}
				break;
			}
		}
	}
	else if (!_logFile.open(_filename, O_RDWR | O_CREAT | O_AT_END)) {
		BatLib_Error(ERR_SD_OPEN);
	}
}

int	BatLog::printf(const char * fromatString, ...)
{
	va_list argptr;
	va_start(argptr, fromatString);
	char buffer[150];
	vsprintf_P(buffer, fromatString, argptr);
	va_end(argptr);
	_logFile.println(buffer);
}

void BatLog::stop()
{
	_logFile.close();
	delay(200);
	digitalWrite(PIN_SD_PWR, LOW);

	//Disable all SPI Pins
	pinMode(MOSI, INPUT_PULLUP);
	pinMode(MISO, INPUT_PULLUP);
	pinMode(SCK, INPUT_PULLUP);
	pinMode(PIN_SD_CS, INPUT_PULLUP);

	if (_indicatorLedPin >= 0){
		digitalWrite(_indicatorLedPin, LOW);
	}
}


