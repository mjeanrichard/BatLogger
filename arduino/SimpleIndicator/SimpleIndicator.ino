
const int PIN_LED_GREEN = 3;
const int PIN_LED_YELLOW = 6;
const int PIN_LED_RED = 9;

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

	initCounter();
}

void loop()
{
        readCounter();
	int led = 0;
        unsigned long startTime=0;
        unsigned long sum=0;
        unsigned int maxFreqKhz=0;
        
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
/*                  if (sum > 0 && maxFreqKhz > 9 && diff > 5000){
                    Serial.print(maxFreqKhz);
                    Serial.print(" - ");
                    Serial.println(diff);
                  }*/
                  sum = 0;
                  maxFreqKhz = 0;
                  continue;
                }
                //Serial.println(val);
                if (sum == 0){
                  //Start of Call?
                  sum = val;
                  startTime = time;
                  maxFreqKhz=0;
                  continue;
                }
                
                sum += val;
                unsigned long diff = time - startTime;
                unsigned int freqKhz = (sum * 1000) / diff;
//                Serial.println(freqKhz);
                
                if (freqKhz > maxFreqKhz){
                  maxFreqKhz = freqKhz;
                }
                
                if (freqKhz < maxFreqKhz / 2){
/*                  Serial.print(freqKhz);
                  Serial.print(" < ");
                  Serial.print(maxFreqKhz);
                  Serial.print(" - ");
                  Serial.println(diff); */
                  sum = 0;
                  maxFreqKhz = 0;
                  continue;
                }
                
                
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
	}
}

