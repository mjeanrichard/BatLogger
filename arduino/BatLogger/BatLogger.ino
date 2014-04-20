//#define DEBUG

#include <Wire.h>
#include "RTClib.h"
#include <SD.h>

RTC_DS1307 RTC;

//Board configuration
const int BatInputPin = 2;
const int Button1Pin = 8;
const int Button2Pin = 9;

const int YellowLedPin = 6;
const int RedLedPin = 7;
const int GreenLedPin = 5;

const int SDShieldSwitchPin = 3;
const int SDPin = 10;

//Time of Silence to detect End of Call in microseconds
const int CallEndTimeout = 5000;

// Anzahl Datensaetze die im speicher gehalten werden, bevor auf die Karte geschrieben wird.
const byte LogBufferSize = 10;

volatile unsigned long callStartMicro;
volatile unsigned long callStartMillis;
volatile unsigned long lastCallMicro;


//index auf die aktuelle Buffer position
byte index = 0;
byte pulseStore[LogBufferSize];
unsigned int durationStore[LogBufferSize];
unsigned long startTimeStore[LogBufferSize];

char filename[] = "LOG00.CSV";

// Interrupt Variable um BatInputs zu zählen
volatile int pulses = 0;

void setup(){

  pinMode(BatInputPin, INPUT);
  pinMode(Button1Pin, INPUT);
  pinMode(Button2Pin, INPUT);
  pinMode(YellowLedPin, OUTPUT);
  pinMode(RedLedPin, OUTPUT);
  pinMode(GreenLedPin, OUTPUT);
  pinMode(SDShieldSwitchPin, OUTPUT);
  analogReference( DEFAULT );

  #ifdef DEBUG
    Serial.begin(57600);
  #endif

  initSdCard();

  attachInterrupt(0, count, CHANGE); 
  
  indicatorMode();
}

void loop(){
  pulses = 0;

  int tempPulses = 0;
  while(pulses == 0 || pulses > tempPulses || (micros() - lastCallMicro) < CallEndTimeout){
    if (digitalRead(Button2Pin) == LOW){
      writeLog();
      indicatorMode();
    }
    tempPulses = pulses;
    delay(1);
  }
  
  #ifdef DEBUG
    Serial.print(F("Free Ram: "));
    Serial.println(freeRam());
  #endif
  
  digitalWrite(GreenLedPin, LOW);
  int diff = lastCallMicro - callStartMicro;
  if (tempPulses > 1 && diff > 0)
  {
    int f = (tempPulses * 16) / (diff / 1000);
    if (f > 5){
      digitalWrite(GreenLedPin, HIGH);
      pulseStore[index] = tempPulses;
      durationStore[index] = diff;
      startTimeStore[index] = callStartMillis;

      index++;
      if (index >= LogBufferSize){
        writeLog();
      } 
    }
  }
}

void setLeds(int green, int yellow, int red){
  digitalWrite(GreenLedPin, green);
  digitalWrite(YellowLedPin, yellow);
  digitalWrite(RedLedPin, red);
}

void indicatorMode(){
  boolean abort = false;

  setLeds(LOW, LOW, HIGH);
  delay(250);
  setLeds(LOW, HIGH, HIGH);
  delay(250);
  setLeds(HIGH, HIGH, HIGH);
  delay(250);
  setLeds(LOW, LOW, LOW);
  callStartMillis = 0;

  while(!abort)
  {
    unsigned long diff = millis() - callStartMillis;
    pulses=0;
    if (diff < 5){
      setLeds(HIGH, HIGH, HIGH);
    } 
    else if (diff < 150){
      setLeds(LOW, HIGH, HIGH);
    }
    else if (diff < 300){
      setLeds(LOW, LOW, HIGH);
    } else {
      setLeds(LOW, LOW, LOW);
    }
    abort = digitalRead(Button1Pin) == LOW;
  }
  setLeds(HIGH, HIGH, HIGH);
  delay(250);
  setLeds(HIGH, HIGH, LOW);
  delay(250);
  setLeds(HIGH, LOW, LOW);
  delay(250);
  setLeds(LOW, LOW, LOW);
}


void writeLog(){
  digitalWrite(YellowLedPin, HIGH);
  digitalWrite(SDShieldSwitchPin, HIGH);
  delay(500);
  File logfile = SD.open(filename, FILE_WRITE); 
  for (int i = 0; i < index; i++){
    logfile.print(startTimeStore[i]);
    logfile.print(';');
    logfile.print(pulseStore[i]);
    logfile.print(';');
    logfile.println(durationStore[i]);
  }
  logfile.close();

  index = 0;
  digitalWrite(SDShieldSwitchPin, LOW);
  digitalWrite(YellowLedPin, LOW);
}

int freeRam () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

void count()
{
  if (pulses == 0)
  {
    callStartMicro = micros();
    callStartMillis = millis();
  }
  lastCallMicro = micros();
  pulses++;
}


void initSdCard()
{
  digitalWrite(YellowLedPin, HIGH);
  digitalWrite(SDShieldSwitchPin, HIGH);
  delay(500);

  Wire.begin();
  RTC.begin();

  if (! RTC.isrunning()) {
    error(1);
  }

  pinMode(SDPin, OUTPUT);
  if (!SD.begin(SDPin)) {
    error(3);
  }

  File logfile;
  for (int i = 0; i < 100; i++) {
    filename[3] = i/10 + '0';
    filename[4] = i%10 + '0';
    if (!SD.exists(filename)) {
      logfile = SD.open(filename, FILE_WRITE); 
      break;
    }
  }

  if (!logfile) {
    error(4);
  }

  DateTime now = RTC.now();
  unsigned long nowMillis = millis();

  char buffer[150];
  sprintf_P(buffer, PSTR("%lu;%02d/%02d/%02d %02d:%02d:%02d"), nowMillis, now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
  logfile.println(buffer);
  
  sprintf_P(buffer, PSTR("start time;pulses;duration"));
  logfile.println(buffer);
  
  logfile.close();
  digitalWrite(SDShieldSwitchPin, LOW);
  digitalWrite(YellowLedPin, LOW);
}



void error(int code){
  while(true)
  {
    for(int i=0; i < code; i++){
      digitalWrite(RedLedPin, HIGH);
      delay(250);
      digitalWrite(RedLedPin, LOW);
      delay(500);
    }    
    delay(2000);
  };
}

