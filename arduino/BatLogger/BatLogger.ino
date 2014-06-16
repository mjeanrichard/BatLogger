//#define RTC_ENABLED

#include <SdFat.h>
#include <EEPROM.h>
#include <avr/wdt.h>
#include <avr/sleep.h>

#include "BatLog.h"
#include "BatLib.h"

//#ifdef RTC_ENABLED
//#include <Wire.h>
//#include "RTClib.h"
//RTC_DS1307 RTC;
//#endif

BatLog Log;
BatBoardClass BatBoard;

//Time of Silence to detect End of Call in microseconds
const int CallEndTimeout = 5000;

// Interrupt Variablen um BatInputs zu zählen
volatile int pulses = 0;
volatile unsigned long callStartMicro;
volatile unsigned long callStartMillis;
volatile unsigned long lastCallMicro;

// Anzahl Datensaetze die im speicher gehalten werden, bevor auf die Karte geschrieben wird.
const byte LogBufferSize = 10;
//index auf die aktuelle Buffer position
byte index = 0;
byte pulseStore[LogBufferSize];
unsigned int durationStore[LogBufferSize];
unsigned long startTimeStore[LogBufferSize];

bool isLogging = false;

unsigned long lastLowCall;
unsigned long lastMidCall;
unsigned long lastHighCall;

void loop()
{
	pulses = 0;

	int tempPulses = 0;
	while (pulses == 0 || pulses > tempPulses || (micros() - lastCallMicro) < CallEndTimeout){
		if (digitalRead(PIN_S2) == LOW){
			toggleIndicatorMode();
		}
		tempPulses = pulses;
		checkVcc();
		if (!isLogging){
			updateIndicatorLed(PIN_LED_GREEN, lastHighCall);
			updateIndicatorLed(PIN_LED_YELLOW, lastMidCall);
			updateIndicatorLed(PIN_LED_RED, lastLowCall);
		}
	}

	unsigned int diff = lastCallMicro - callStartMicro;
	if (tempPulses > 1 && diff > 0)
	{
		unsigned int f = (tempPulses * 16 * 1000000L)  / diff;
		if (isLogging)
		{
			if (f > 2000) 
			{
				store(tempPulses, diff, callStartMillis);
			}
		}
		else
		{
			indicate(f);
		}
	}

}

void store(byte pulseCount, unsigned int duration, unsigned long startMs){
	pulseStore[index] = pulseCount;
	durationStore[index] = duration;
	startTimeStore[index] = startMs;

	index++;
	if (index >= LogBufferSize){
		writeBufferToLog();
	}

}

void indicate(unsigned int f)
{
	if (f < 5000) {
		lastLowCall = millis();
		updateIndicatorLed(PIN_LED_RED, lastLowCall);
	}
	else if (f < 20000) {
		lastMidCall = millis();
		updateIndicatorLed(PIN_LED_YELLOW, lastMidCall);
	}
	else {
		lastHighCall = millis();
		updateIndicatorLed(PIN_LED_GREEN, lastHighCall);
	}
}

void updateIndicatorLed(int pin, unsigned long lastCall)
{
	unsigned long diff = millis() - lastCall;

	if (diff > 500){
		digitalWrite(pin, LOW);
		return;
	}

	byte value = 255 - (diff / 2);
	analogWrite(pin, value);
}

void toggleIndicatorMode()
{
	if (isLogging){
		writeBufferToLog();
		isLogging = false;
		lastHighCall = 0;
		lastMidCall = 0;
		lastLowCall = 0;
		BatLib_SetLeds(false, false, true);
		delay(250);
		BatLib_SetLeds(false, true, true);
		delay(250);
		BatLib_SetLeds(true, true, true);
		delay(250);
		BatLib_SetLeds(false, false, false);
	}
	else
	{
		isLogging = true;
		BatLib_SetLeds(true, true, true);
		delay(250);
		BatLib_SetLeds(true, true, false);
		delay(250);
		BatLib_SetLeds(true, false, false);
		delay(250);
		BatLib_SetLeds(false, false, false);
	}
}

void setup()
{
	//Serial.begin(9600);

	// Disable unneeded devices
	// 7 - PRTWI: Power Reduction TWI
	// 6 - PRTIM2 : Power Reduction Timer / Counter2
	// 5 - PRTIM0 : Power Reduction Timer / Counter0
	// 4 - Res : Reserved bit
	// 3 - PRTIM1 : Power Reduction Timer / Counter1
	// 2 - PRSPI : Power Reduction Serial Peripheral Interface
	// 1 - PRUSART0 : Power Reduction USART0
	// 0 - PRADC : Power Reduction ADC
	//PRR = B11011010;

	BatBoard.init();
	Log.init(PIN_LED_YELLOW);

	Log.start();
	Log.printf(PSTR("#start time;pulses;duration"));
	Log.stop();

	attachInterrupt(0, callInterrupt, CHANGE);
}

void checkVcc()
{
	long vcc = BatBoard.getVcc();
	if (vcc < 3800){
		writeBufferToLog();
		Log.start();
		Log.printf(PSTR("# %lu -> Power failure: %li mV"), millis(), vcc);
		Log.stop();
		while (true){
			analogWrite(PIN_LED_YELLOW, 50);
			delay(500);
			digitalWrite(PIN_LED_YELLOW, LOW);
			sleep();
		}
	}
}

// == Interrupt Method to count Bat-Calls
void callInterrupt()
{
	if (pulses == 0)
	{
		callStartMicro = micros();
		callStartMillis = millis();
	}
	lastCallMicro = micros();
	pulses++;
}


// == Writes the Memory-Buffer to the LogFile and clears the Buffer.
void writeBufferToLog()
{
	Log.start();
	for (int i = 0; i < index; i++){
		Log.printf(PSTR("%lu;%hhu;%u"), startTimeStore[i], pulseStore[i], durationStore[i]);
	}
	Log.stop();
	index = 0;
}

// Put Arduino to deep Sleep for 8 secs
void sleep()
{
	BatLib_SetLeds(false, false, false);
	detachInterrupt(0);

	byte oldAdcsra = ADCSRA;

	//disable ADC
	ADCSRA = 0;
	// clear various "reset" flags
	MCUSR = 0;
	// allow changes, disable reset
	WDTCSR = bit(WDCE) | bit(WDE);
	// set interrupt mode and an interval 
	WDTCSR = bit(WDIE) | bit(WDP3) | bit(WDP0);    // set WDIE, and 8 seconds delay
	wdt_reset();  // pat the dog

	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();

	// turn off brown-out enable in software
	MCUCR = bit(BODS) | bit(BODSE);
	MCUCR = bit(BODS);
	sleep_cpu();

	// cancel sleep as a precaution
	sleep_disable();

	//Restore Values
	ADCSRA = oldAdcsra;
	attachInterrupt(0, callInterrupt, CHANGE);
}

// Watchdog timer interrupt.
ISR(WDT_vect)
{
	wdt_disable();
}
