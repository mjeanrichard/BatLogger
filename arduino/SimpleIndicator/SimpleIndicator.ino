#include <SdFat.h>
SdFat sd;

const int PIN_LED_GREEN = 3;
const int PIN_LED_YELLOW = 6;
const int PIN_LED_RED = 9;

const int SDShieldSwitchPin = 7;
const int SDPin = 10;

// Anzahl Datensaetze die im speicher gehalten werden, bevor auf die Karte geschrieben wird.
const byte LogBufferSize = 100;
//index auf die aktuelle Buffer position
byte index = 0;

/*unsigned long pulseStore[LogBufferSize];
unsigned long durationStore[LogBufferSize];
unsigned long startTimeStore[LogBufferSize];
unsigned int maxFreqStore[LogBufferSize];
*/


unsigned int timeSeries[LogBufferSize];
unsigned int pulseSeries[LogBufferSize];
unsigned int loudSeries[LogBufferSize];
int seriesIndex = 0;


//char filename[] = "LOG--.CSV";


unsigned long readCounter(){
	unsigned int value = 0;
	unsigned int of = 0;

	TCCR1B = 0;
	value = TCNT1;
	of = TIFR1 & (1 << TOV1);
	TCNT1 = 0;
	TIFR1 = (1 << TOV1);
	TCCR1B = (1 << CS12) | (1 << CS11) | (1 << CS10);
	if (of){
		return value + 65536UL;
	}
	return value;
}

void initCounter()
{
	TCCR1B = 0;
	TCCR1A = 0;
	TCNT1 = 0;
	TIFR1 = (1 << TOV1);
	TIMSK1 = 0;

	// External clock source on T1 pin. Clock on rising edge.
	TCCR1B = (1 << CS12) | (1 << CS11) | (1 << CS10);
}


void setup()
{
	Serial.begin(57600);
  
	pinMode(PIN_LED_GREEN, OUTPUT);
	pinMode(PIN_LED_YELLOW, OUTPUT);
	pinMode(PIN_LED_RED, OUTPUT);

	pinMode(A0, INPUT);

	/*SdFile file = openLogFile();
	file.println("#StartUp completed");
	closeLogFile(file);*/

	initCounter();
}

void loop()
{
        readCounter();
	int led = 0;
        unsigned long startTime=micros();
        unsigned long lastTime=startTime;
        unsigned long sum=0;
        unsigned int maxFreqKhz=0;
        int loud=0;
        
 	while (true)
	{
		delayMicroseconds(1000);
		unsigned long val = readCounter();
                unsigned long time = micros();

                if (led > 0) {
                  digitalWrite(led, LOW);
                }
		led = 0;
                
                if (val < 10){
                  unsigned long diff = time - startTime;
                  if (sum > 0 && maxFreqKhz > 9 && diff > 5000){
                    //logData(sum, startTime, diff, maxFreqKhz, loud);
                    logSeries();
                  }
                  sum = 0;
                  maxFreqKhz = 0;
                  seriesIndex = 0;
                  lastTime=time;
                  startTime=time;
                  continue;
                }
                
                sum += val;
                unsigned long diff = time - startTime;
                unsigned int freqKhz = (sum * 1000) / diff;
                
                timeSeries[seriesIndex] = time-lastTime;
                pulseSeries[seriesIndex] = val;
                loudSeries[seriesIndex] = analogRead(A0);

                
                if (freqKhz > maxFreqKhz){
                  maxFreqKhz = freqKhz;
                }
                
/*                if (freqKhz < maxFreqKhz / 4){
                  //logData(sum, startTime, diff, maxFreqKhz, loud);
                  logSeries();
                  sum = 0;
                  maxFreqKhz = 0;
                  continue;
                }
*/
                seriesIndex++;
                lastTime=time;
/*                
                if (maxFreqKhz > 25UL){
		  led = PIN_LED_GREEN;
		}
		else if (maxFreqKhz > 15UL){
		  led = PIN_LED_YELLOW;
		}
		else
		{
		  led = PIN_LED_RED;
		}
		if (led > 0) {
                  digitalWrite(led, HIGH);
                }
*/
	}
}
/*
void logData(unsigned long sum, unsigned long startTime, unsigned long diff, unsigned int maxFreqKhz, int loud)
{
  Serial.print(maxFreqKhz);
  Serial.print(" - ");
  Serial.print(loud);
  Serial.print(" - ");
  Serial.print(diff);
  Serial.print(" - ");
  Serial.println(sum);
  
  /*pulseStore[index] = sum;
  durationStore[index] = diff;
  startTimeStore[index] = startTime;
  maxFreqStore[index] = maxFreqKhz;
  index++;
  if (index >= LogBufferSize){
    writeBufferToLog();
  }
}
*/
void logSeries(){
  Serial.println("------------------------------");
  for (int i = 0; i < seriesIndex; i++){
    Serial.print(i);
    Serial.print(';');
    Serial.print(timeSeries[i]);
    Serial.print(';');
    Serial.print(pulseSeries[i]);
    Serial.print(';');
    Serial.println(loudSeries[i]);
  }
}
/*
// == Writes the Memory-Buffer to the LogFile and clears the Buffer.
void writeBufferToLog()
{
  SdFile logfile = openLogFile();
  for (int i = 0; i < index; i++){
    logfile.print(startTimeStore[i]);
    logfile.print(';');
    logfile.print(pulseStore[i]);
    logfile.print(';');
    logfile.print(durationStore[i]);
    logfile.print(';');
    logfile.println(maxFreqStore[i]);
  }
  closeLogFile(logfile);
  index = 0;
}

// == Opens the Logfile (and initializes if new)
SdFile openLogFile(){
  digitalWrite(PIN_LED_RED, HIGH);
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
  digitalWrite(PIN_LED_RED, LOW);
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

  unsigned long nowMillis = millis();
  char buffer[150];

  sprintf_P(buffer, PSTR("#start time;pulses;duration;maxFreq"));
  logfile.println(buffer);
  return logfile;
}

// == Stops PrgrammExecution and Reports the ErrorCode with the red LED
void error(int code){
  while(true)
  {
    for(int i=0; i < code; i++){
      digitalWrite(PIN_LED_RED, HIGH);
      delay(250);
      digitalWrite(PIN_LED_RED, LOW);
      delay(500);
    }    
    delay(2000);
  };
}
*/
