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

// Anzahl Datensaetze die im speicher gehalten werden, bevor auf die Karte geschrieben wird.
const byte LogBufferSize = 50;
//index auf die aktuelle Buffer position
byte index = 0;
unsigned int pulseStore[LogBufferSize];
unsigned long durationStore[LogBufferSize];
unsigned long startTimeStore[LogBufferSize];

bool isLogging = false;

unsigned long lastLowCall;
unsigned long lastMidCall;
unsigned long lastHighCall;

const int DelayMs = 5000;
// => f[kHz] * t[ms]
const int MinPulsesPerDelay = 3 * 5;



void loop()
{
	Serial.begin(57600);
	while (true)
	{
		BatBoard.readCounter();
		delayMicroseconds(10000);
		unsigned long val = BatBoard.readCounter()*2;
		Serial.println(val);
		int led = 0;
		if (val > 200UL){
			led = PIN_LED_GREEN;
		}
		else if (val > 100UL){
			led = PIN_LED_YELLOW;
		}
		else if (val > 30UL) 
		{
			led = PIN_LED_RED;
		}
		digitalWrite(led, HIGH);
		delay(200);
		digitalWrite(led, LOW);
	}


	unsigned long totalCount = 0;
	unsigned long startTime = micros();
	unsigned long lastTime = startTime;
	unsigned long endTime = startTime;

	BatBoard.readCounter();
	while (true)
	{
		if (digitalRead(PIN_S2) == LOW){
			toggleIndicatorMode();
		}

		delayMicroseconds(DelayMs);
		unsigned long counterValue = BatBoard.readCounter();
		lastTime = micros();

		if (counterValue < MinPulsesPerDelay){
			//End Of Pulse
			unsigned long timeDiff = endTime - startTime;
			if (totalCount > MinPulsesPerDelay){
				//Valid Pulse, calculate
				unsigned long f = (totalCount * 1000000) / (timeDiff);

				if (isLogging && f > 3000){
					pulseStore[index] = totalCount;
					durationStore[index] = timeDiff;
					startTimeStore[index] = startTime;

					index++;
					if (index >= LogBufferSize){
						writeBufferToLog();
						delay(50);
						//Reset Counter after writing to the SDCard
						BatBoard.readCounter();
						lastTime = micros();
					}
				}

				if (!isLogging) {
					indicate(f);
				}
			}
			totalCount = 0;
			startTime = lastTime;
		}
		else {
			totalCount += counterValue;
		}
		endTime = lastTime;

		if (!isLogging){
			updateIndicatorLed(PIN_LED_GREEN, lastHighCall);
			updateIndicatorLed(PIN_LED_YELLOW, lastMidCall);
			updateIndicatorLed(PIN_LED_RED, lastLowCall);
		}

		checkVcc();
	}
}

void indicate(unsigned int f)
{
	if (f < 3000) {
		lastLowCall = millis();
		updateIndicatorLed(PIN_LED_RED, lastLowCall);
	}
	else if (f < 10000) {
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

	if (diff > 200){
		digitalWrite(pin, LOW);
	}
	else {
		digitalWrite(pin, HIGH);
	}
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
	//PRR = B11000010;

	BatBoard.init();
	//Log.init(PIN_LED_YELLOW);

	//Log.start();
	//Log.printf(PSTR("#start time;pulses;duration"));
	//Log.stop();
}



void checkVcc()
{
	long vcc = BatBoard.getVcc();
	if (vcc < 4600){
		writeBufferToLog();
		Log.start();
		Log.printf(PSTR("# %lu -> Power failure: %li mV"), millis(), vcc);
		Log.stop();
		while (true){
			digitalWrite(PIN_LED_YELLOW, HIGH);
			delay(500);
			digitalWrite(PIN_LED_YELLOW, LOW);
			sleep();
		}
	}
}


// == Writes the Memory-Buffer to the LogFile and clears the Buffer.
void writeBufferToLog()
{
	Log.start();
	for (int i = 0; i < index; i++){
		Log.printf(PSTR("%lu;%u;%lu"), startTimeStore[i], pulseStore[i], durationStore[i]);
	}
	Log.stop();
	index = 0;
}

// Put Arduino to deep Sleep for 8 secs
void sleep()
{
	BatLib_SetLeds(false, false, false);

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
}

// Watchdog timer interrupt.
ISR(WDT_vect)
{
	wdt_disable();
}
