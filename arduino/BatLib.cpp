
#include "BatLib.h"

BatBoardClass::BatBoardClass()
{}

void BatBoardClass::init()
{
	pinMode(PIN_LED_GREEN, OUTPUT);
	pinMode(PIN_LED_YELLOW, OUTPUT);
	pinMode(PIN_LED_RED, OUTPUT);

	pinMode(PIN_BAT_IN, INPUT);

	pinMode(PIN_S1, INPUT);
	digitalWrite(PIN_S1, HIGH);

	pinMode(PIN_S2, INPUT);
	digitalWrite(PIN_S2, HIGH);

	pinMode(PIN_SD_PWR, OUTPUT);


}

// Helper Method to Set the State of all LEDs
void BatBoardClass::setLeds(int green, int yellow, int red)
{
	digitalWrite(PIN_LED_GREEN, green);
	digitalWrite(PIN_LED_YELLOW, yellow);
	digitalWrite(PIN_LED_RED, red);
}

// Stops PrgrammExecution and Reports the ErrorCode with the red LED
void BatBoardClass::error(int code){
	while (true)
	{
		for (int i = 0; i < code; i++){
			setLeds(LOW, LOW, HIGH);
			delay(250);
			setLeds(LOW, LOW, LOW);
			delay(500);
		}
		delay(2000);
	};
}


long BatBoardClass::getVcc()
{
	long voltRef = getRefVoltageSetting();
	return getVcc(voltRef);
}

long BatBoardClass::getVcc(long refMiliVoltage)
{
	// Read 1.1V reference against AVcc
	// set the reference to Vcc and the measurement to the internal 1.1V reference
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
	ADMUX = _BV(MUX5) | _BV(MUX0);
#elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
	ADMUX = _BV(MUX3) | _BV(MUX2);
#else
	ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#endif  

	delay(2); // Wait for Vref to settle
	ADCSRA |= _BV(ADSC); // Start conversion
	while (bit_is_set(ADCSRA, ADSC)); // measuring

	uint8_t low = ADCL; // must read ADCL first - it then locks ADCH  
	uint8_t high = ADCH; // unlocks both

	long result = (high << 8) | low;

	result = (refMiliVoltage * 1023) / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
	return result; // Vcc in millivolts
}

long BatBoardClass::getRefVoltageSetting()
{
	long result = 1100;
	EEPROM_read(0, result);
	return result;
}


BatBoardClass BatBoard;

