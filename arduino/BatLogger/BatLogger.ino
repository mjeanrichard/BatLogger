//#define DEBUG
//#define RTC_ENABLED

#include <SdFat.h>
SdFat sd;

#ifdef RTC_ENABLED
  #include <Wire.h>
  #include "RTClib.h"
  RTC_DS1307 RTC;
#endif


//Board configuration
const int BatInputPin = 2;
const int Button1Pin = 8;
const int Button2Pin = 9;

const int GreenLedPin = 5;
const int YellowLedPin = 6;
const int RedLedPin = 7;

const int SDShieldSwitchPin = 3;
const int SDPin = 10;

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

char filename[] = "LOG--.CSV";

void setup()
{
  pinMode(BatInputPin, INPUT);
  pinMode(Button1Pin, INPUT);
  digitalWrite(Button1Pin, HIGH);
  pinMode(Button2Pin, INPUT);
  digitalWrite(Button2Pin, HIGH);
  pinMode(YellowLedPin, OUTPUT);
  pinMode(RedLedPin, OUTPUT);
  pinMode(GreenLedPin, OUTPUT);
  pinMode(SDShieldSwitchPin, OUTPUT);
  analogReference( DEFAULT );

#ifdef DEBUG
  Serial.begin(57600);
#endif

  attachInterrupt(0, callInterrupt, CHANGE); 

  SdFile file = openLogFile();
  file.println("#StartUp completed");
  closeLogFile(file);

  indicatorMode();
}

void loop()
{
  pulses = 0;

  int tempPulses = 0;
  while(pulses == 0 || pulses > tempPulses || (micros() - lastCallMicro) < CallEndTimeout){
    if (digitalRead(Button2Pin) == LOW){
      writeBufferToLog();
      indicatorMode();
    }
    tempPulses = pulses;
    delay(1);
  }

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
        writeBufferToLog();
      } 
    }
  }
}

void indicatorMode()
{
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
    } 
    else {
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

// == Writes the Memory-Buffer to the LogFile and clears the Buffer.
void writeBufferToLog()
{
  SdFile logfile = openLogFile();
  for (int i = 0; i < index; i++){
    logfile.print(startTimeStore[i]);
    logfile.print(';');
    logfile.print(pulseStore[i]);
    logfile.print(';');
    logfile.println(durationStore[i]);
  }
  closeLogFile(logfile);
  index = 0;
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

// == Opens the Logfile (and initializes if new)
SdFile openLogFile(){
  digitalWrite(YellowLedPin, HIGH);
  digitalWrite(SDShieldSwitchPin, HIGH);
  delay(200);

  if (!sd.begin(SDPin, SPI_FULL_SPEED)){
    error(3);
  }

  SdFile logfile;
  if (!sd.exists(filename)) {
    logfile = initializeNewLog();
  } 
  else if (!logfile.open(filename, O_RDWR | O_CREAT | O_AT_END)) {
    error(4);
  } 
  return logfile;
}

// == Closes LogFile and turns the LED and the SD Module off.
void closeLogFile(SdFile logfile)
{
  logfile.close();
  delay(200);
  digitalWrite(SDShieldSwitchPin, LOW);
  digitalWrite(YellowLedPin, LOW);
}

// == Initializese a new LogFile with Headers.
SdFile initializeNewLog()
{
  SdFile logfile;
  for (int i = 0; i < 100; i++) {
    filename[3] = i/10 + '0';
    filename[4] = i%10 + '0';
    if (!sd.exists(filename)) {
      if (!logfile.open(filename, O_RDWR | O_CREAT | O_AT_END)) {
        error(4);
      } 
      break;
    }
  }

#ifdef RTC_ENABLED
  RTC.begin();

  if (! RTC.isrunning()) {
    error(1);
  }
#endif

  unsigned long nowMillis = millis();
  char buffer[150];

#ifdef RTC_ENABLED
  DateTime now = RTC.now();
  sprintf_P(buffer, PSTR("#%lu;%02d/%02d/%02d %02d:%02d:%02d"), nowMillis, now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
  logfile.println(buffer);
#endif

  sprintf_P(buffer, PSTR("#start time;pulses;duration"));
  logfile.println(buffer);
  return logfile;
}

// == Stops PrgrammExecution and Reports the ErrorCode with the red LED
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

// == Helper Method to Set the State of all LEDs
void setLeds(int green, int yellow, int red){
  digitalWrite(GreenLedPin, green);
  digitalWrite(YellowLedPin, yellow);
  digitalWrite(RedLedPin, red);
}



