#include "BatLib.h"
#include <EEPROM.h>
#include <SdFat.h>

#include <EEPROM.h>
#include <SdFat.h>
SdFat sd;

const int ADR_REF_VOLT = 0;

void loop(){

	Serial.println();
	Serial.println("====== MENU ==================");
	Serial.println("1. Configure reference Voltage");
	Serial.println("2. ----");
	Serial.println("3. Show current Configuration");
	Serial.println();
	Serial.println("Please select:");
	String input = readToNewLine();

	if (input.length() == 1)
	{
		switch (input.charAt(0))
		{
		case '1':
			setReferenceVoltage();
			break;
		case '3':
			printConfiguration();
			break;
		}
	}
}

void setReferenceVoltage(){
	Serial.println();
	Serial.println("Enter current Voltage [mV]:");
	long curVolt = readToNewLine().toInt();

	long uncalibrated = readVcc(1100);
	long refMiliVoltage = (1100 * curVolt) / uncalibrated;
	saveLong(ADR_REF_VOLT, refMiliVoltage);
	long miliVolts = readVcc(refMiliVoltage);
	Serial.print("Calibrated Voltage [mV]: ");
	Serial.println(miliVolts);
}

void printConfiguration(){
	Serial.println();
	Serial.println("Current configuration:");

	long refVolt = loadLong(ADR_REF_VOLT);
	long miliVolts = readVcc(refVolt);
	Serial.print("Reference Voltage [mV]:    ");
	Serial.print(refVolt);
	Serial.print(" (Current [mV]: ");
	Serial.print(miliVolts);
	Serial.println(")");
}

void setup(){
	pinMode(7, OUTPUT);

	Serial.begin(9600);
}

void saveLong(int adress, long value)
{
	const byte* p = (const byte*)&value;
	unsigned int i;
	for (i = 0; i < sizeof(value); i++)
	{
		EEPROM.write(adress + i, *p++);
	}
	long checkValue = loadLong(adress);
	if (value != checkValue)
	{
		Serial.print("Could not write Value!");
	}
}

long loadLong(int adress)
{
	long result = 0;
	byte* p = (byte*)&result;
	unsigned int i;
	for (i = 0; i < sizeof(result); i++)
	{
		*p++ = EEPROM.read(adress++);
	}
	return result;
}



long readVcc(long refMiliVoltage) {
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

String readToNewLine(){
	String inString;
	while (true) {
		char inChar = Serial.read();
		if (isDigit(inChar)) {
			inString += inChar;
		}
		if (inChar == '\n') {
			return inString;
		}
	}
}

