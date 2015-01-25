
#include "SdFat.h"

static uint8_t saveTCCR1A, saveTCCR1B;

static inline void counter_init(void)
{
	saveTCCR1A = TCCR1A;
	saveTCCR1B = TCCR1B;
	TCCR1B = 0;
	TCCR1A = 0;
	TCNT1 = 0;
	TIFR1 = (1 << TOV1);
	TIMSK1 = 0;
}

static inline void counter_start(void)
{
	// External clock source on T1 pin. Clock on rising edge.
	TCCR1B = (1 << CS12) | (1 << CS11) | (1 << CS10);
}

	
const int DelayMs = 5000;

// => f[kHz] * t[ms]
const int MinPulsesPerDelay = 3*5;

const int gLed = 3;
const int yLed = 6;
const int rLed = 9;


void setup() {
	//Serial.begin(9600);
	pinMode(9, OUTPUT);
	pinMode(6, OUTPUT);
	pinMode(3, OUTPUT);
	pinMode(7, OUTPUT);
	digitalWrite(7, HIGH);

	counter_init();
	counter_start();
}

unsigned long readCounter(){
	unsigned int value = 0;
	unsigned int overflow = 0;
	
	TCCR1B = 0;
	value = TCNT1;
	overflow = TIFR1 & (1 << TOV1);
	TCNT1 = 0;
	TIFR1 = (1 << TOV1);
	TCCR1B = (1 << CS12) | (1 << CS11) | (1 << CS10);
	if (overflow){
		return value + 65536UL;
	}
	return value;
}

// Anzahl Datensaetze die im speicher gehalten werden, bevor auf die Karte geschrieben wird.
const byte LogBufferSize = 10;
//index auf die aktuelle Buffer position
byte index = 0;
unsigned long pulseStore[LogBufferSize];
unsigned long durationStore[LogBufferSize];
unsigned long startTimeStore[LogBufferSize];
SdFat _sd;
SdFile _logFile;


void loop() {
	readCounter();

	//unsigned long a = 0;
	//unsigned long b = 0;
	//unsigned long t = 0;
	//unsigned long p = 0;
	//int i = 0;
	//while (true)
	//{
	//	unsigned long val1 = readCounter();
	//	a = micros();

	//	delayMicroseconds(5000);
	//	
	//	unsigned long val2 = readCounter();
	//	b = micros();

	//	i++;
	//	t += b - a;
	//	p += val2;

	//	if (i >= 0){
	//		//Serial.print("T: ");
	//		//Serial.print(t);
	//		//Serial.print(" P: ");
	//		//Serial.println(p);

	//		int led = 0;
	//		if (p > 20){
	//			led = rLed;
	//		}
	//		else if (p > 15){
	//			led = yLed;
	//		}
	//		else {
	//			led = gLed;
	//		}
	//		digitalWrite(led, HIGH);
	//		delay(200);
	//		digitalWrite(led, LOW);

	//		t = 0UL;
	//		p = 0UL;
	//		i = 0UL;
	//	}
	//}



	unsigned long totalCount = 0;
	unsigned long startTime = micros();
	unsigned long lastTime = startTime;
	unsigned long endTime = startTime;

	unsigned long g = 0;
	unsigned long y = 0;
	unsigned long r = 0;

	unsigned long m1 = 0;
	readCounter();
	while (true)
	{
		//Serial.println(millis() - m1);
		//m1 = millis();
		delayMicroseconds(DelayMs);
		unsigned long counterValue = readCounter();
		lastTime = micros();

		//Serial.print("p: ");
		//Serial.println(counterValue);
		if (counterValue < MinPulsesPerDelay){
			//End Of Pulse
			unsigned long timeDiff = endTime - startTime;
			if (totalCount > MinPulsesPerDelay){
				//Serial.print("d: ");
				//Serial.println(totalCount);
				//Serial.print("t: ");
				//Serial.println(timeDiff);
				//Valid Pulse, calculate
				unsigned long f = (totalCount * 1000) / (timeDiff);
				//Serial.print("f: ");
				//Serial.println(f);

				pulseStore[index] = totalCount;
				durationStore[index] = timeDiff;
				startTimeStore[index] = startTime;
				index++;
				if (index >= LogBufferSize){
					writeBufferToLog();
					delay(50);
					readCounter();
					lastTime = micros();
				}


				if (f > 15ul){
					g = millis();
				}
				else if (f > 10ul){
					y = millis();
				}
				else if (f > 5ul){
					r = millis();
				}
			}
			totalCount = 0;
			startTime = lastTime;
		}
		else {
			totalCount += counterValue;
		}
		endTime = lastTime;
		updateLeds(gLed, g);
		updateLeds(yLed, y);
		updateLeds(rLed, r);
	}
}

void updateLeds(int led, unsigned long time)
{
	unsigned long diff = millis() - time;
	if (diff > 500UL) {
		digitalWrite(led, LOW);
		return;
	}
	//int val = 255 - diff / 2;
	//analogWrite(led, val);
	digitalWrite(led, HIGH);
}

char _filename[10] = "LOG--.CSV";
const int PIN_SD_CS = 10;

void logStart()
{
	if (!_sd.begin(PIN_SD_CS, SPI_FULL_SPEED)){
		//Serial.println("E1");
	}

	if (!_sd.exists(_filename)) {
		for (int i = 0; i < 100; i++) {
			_filename[3] = i / 10 + '0';
			_filename[4] = i % 10 + '0';
			//Serial.println(_filename);
			if (!_sd.exists(_filename)) {
				if (!_logFile.open(_filename, O_RDWR | O_CREAT | O_AT_END))
				{
					//Serial.print("E2:");
					//Serial.println(_logFile.getWriteError());
				}
				break;
			}
		}
	}
	else {
		if (!_logFile.open(_filename, O_RDWR | O_CREAT | O_AT_END)){
			//Serial.print("E3:");
			//Serial.println(_logFile.getWriteError());
		}
	}
}

int	logPrintf(const char * fromatString, ...)
{
	va_list argptr;
	va_start(argptr, fromatString);
	char buffer[150];
	vsprintf_P(buffer, fromatString, argptr);
	va_end(argptr);
	_logFile.println(buffer);
}

void logStop()
{
	_logFile.close();
}

void writeBufferToLog()
{
	logStart();
	for (int i = 0; i < index; i++){
		logPrintf(PSTR("%lu;%lu;%lu"), startTimeStore[i], pulseStore[i], durationStore[i]);
	}
	logStop();
	index = 0;
}

