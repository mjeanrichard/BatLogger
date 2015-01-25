const int BatPin = 2;
const int LogPin = 7;

const int CallEndTimeout = 10000; //Time of Silence to detect End of Call in microseconds

const byte LogBufferSize = 50;

unsigned long callStart = 0;
unsigned long callEnd = 0;

volatile int pulses = 0;


byte index = 0;

byte pulseStore[LogBufferSize];
unsigned int durationStore[LogBufferSize];
unsigned int timeSinceLastCallStore[LogBufferSize];
unsigned long lastCallEnd = 0;

void setup(){
  
  pinMode(BatPin, INPUT);
  pinMode(LogPin, OUTPUT);
  analogReference( DEFAULT );
  
  Serial.begin(57600);
  
  attachInterrupt(0, count, CHANGE); 
  
  lastCallEnd = millis();
}

void loop(){
  // Wait for first pulse
  pulses = 0;
  while(pulses == 0){}
  callStart = micros();
  callEnd = callStart;
  
  // Wait for end of call
  int tempPulses = 0;
  while((micros() - callEnd) < CallEndTimeout){
    if (pulses > tempPulses){
      tempPulses = pulses;
      callEnd = micros();
    } 
  }

  if (pulses > 1)
  {
    int diff = callEnd - callStart;
    pulseStore[index] = tempPulses;
    durationStore[index] = diff;
    timeSinceLastCallStore[index] = (callEnd - lastCallEnd) / 1000;

    lastCallEnd = callEnd;
    if (index >= LogBufferSize - 1){
      writeLog();
      index = 0;
    } else {
      index++;
    }
  }
}

void writeLog(){
  digitalWrite(LogPin, HIGH);
  for (int i = 0; i < LogBufferSize; i++){
    Serial.print("-> p: ");
    Serial.print(pulseStore[i]);
    Serial.print(" d: ");
    Serial.print(durationStore[i]);
    Serial.print(" tbc: ");
    Serial.print(timeSinceLastCallStore[i]);
    Serial.print(" f: ");
    Serial.print((pulseStore[i] * 16)/(durationStore[i]/1000.0));
    Serial.println("kHz");
  }
  Serial.print("FreeMem: ");
  Serial.println(freeRam());
  digitalWrite(LogPin, LOW);
}

int freeRam () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

void count(){
  pulses++;
  //lastPulseTime = micros();
}
