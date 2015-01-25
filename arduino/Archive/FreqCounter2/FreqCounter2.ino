const int BatPin = 2;

const int YellowLedPin = 6;
const int RedLedPin = 7;
const int GreenLedPin = 5;

const int CallEndTimeout = 10000; //Time of Silence to detect End of Call in microseconds

volatile unsigned long callStartMicro;
volatile unsigned long callStartMillis;
volatile unsigned long lastCallMicro;
volatile int pulses = 0;

void setup(){
  
  pinMode(BatPin, INPUT);
  digitalWrite(BatPin, HIGH);
  pinMode(YellowLedPin, OUTPUT);
  pinMode(RedLedPin, OUTPUT);
  pinMode(GreenLedPin, OUTPUT);
  analogReference( DEFAULT );
  
  attachInterrupt(0, count, CHANGE); 
  pulses = 0;
}

void loop(){
  // Wait for end of call
  int tempPulses = 0;
  while(pulses == 0 || pulses > tempPulses || (micros() - lastCallMicro) < CallEndTimeout){
    tempPulses = pulses;
    delay(1);
  }

  digitalWrite(GreenLedPin, LOW);
  digitalWrite(YellowLedPin, LOW);
  digitalWrite(RedLedPin, LOW);

  
  pulses = 0;
  int diff = lastCallMicro - callStartMicro;
  if (tempPulses > 1 && diff > 0)
  {
    digitalWrite(GreenLedPin, LOW);
    digitalWrite(YellowLedPin, LOW);
    digitalWrite(RedLedPin, LOW);

    int f = (tempPulses * 16) / (diff / 1000);
    if (f < 5){
      digitalWrite(GreenLedPin, HIGH);
    } else if (f < 10) {
      digitalWrite(YellowLedPin, HIGH);
    } else {
      digitalWrite(RedLedPin, HIGH);
    }
  }
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

