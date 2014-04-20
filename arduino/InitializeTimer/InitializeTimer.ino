// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
 
#include <Wire.h>
#include "RTClib.h"
 
RTC_DS1307 RTC;

const int RedLedPin = 7;
const int GreenLedPin = 5;
const int ButtonPin = 8;

 
void setup () {
  Serial.begin(57600);
  Wire.begin();
  RTC.begin();
  
  pinMode(ButtonPin, INPUT);
  pinMode(RedLedPin, OUTPUT);
  pinMode(GreenLedPin, OUTPUT);

  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
  }
}

boolean isSet = false;
 
void loop () {

    while(!isSet){
      digitalWrite(RedLedPin, HIGH);
      if (digitalRead(ButtonPin) == LOW){
        RTC.adjust(DateTime(2014, 03, 26, 19, 52, 30));
        isSet=true;
        digitalWrite(RedLedPin, LOW);
        digitalWrite(GreenLedPin, HIGH);
      }
    }
  
    DateTime now = RTC.now();
    
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(' ');
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
}
