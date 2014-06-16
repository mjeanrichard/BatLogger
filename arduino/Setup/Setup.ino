#include "BatLib.h"
#include <EEPROM.h>
#include <SdFat.h>

void loop(){
	long miliVolts = BatBoard.getVcc();

	if (miliVolts < 4000){
		BatBoard.setLeds(LOW, LOW, HIGH);
	}
	else if (miliVolts < 4500){
		BatBoard.setLeds(LOW, HIGH, LOW);
	}
	else {
		BatBoard.setLeds(HIGH, LOW, LOW);
	}

	delay(500);
}

void setup(){
	BatBoard.init();
}

