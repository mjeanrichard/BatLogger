#ifndef Morse_h
#define Morse_h

#include "Arduino.h"

int PIN_LED_GREEN = 5;
int PIN_LED_YELLOW = 6;
int PIN_LED_RED = 7;

int PIN_SD_PWR = 3;
int PIN_SD_CS = 10;

int PIN_BAT_IN = 2;

int PIN_S1 = 8;
int PIN_S2 = 9;

class BatBoard
{
  public:
    BatBoard();
    void dot();
    void dash();
  private:
};

#endif
