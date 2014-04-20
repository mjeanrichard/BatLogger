const int BatPin = 2;
const int GrePin = 5;
const int YelPin = 6;
const int RedPin = 7;
const int SDPin = 10;


unsigned long time;

void setup(){

  pinMode(BatPin, INPUT);
  pinMode(YelPin, OUTPUT);
  pinMode(RedPin, OUTPUT);
  pinMode(GrePin, OUTPUT);
  
  attachInterrupt(0, count, CHANGE); 
}

void loop(){
  unsigned long diff = millis() - time;
  
  if (diff < 5){
    digitalWrite(GrePin, HIGH);
    digitalWrite(YelPin, HIGH);
    digitalWrite(RedPin, HIGH);
  } 
  else if (diff < 150){
    digitalWrite(GrePin, LOW);
    digitalWrite(YelPin, HIGH);
    digitalWrite(RedPin, HIGH);
  }
  else if (diff < 300){
    digitalWrite(GrePin, LOW);
    digitalWrite(YelPin, LOW);
    digitalWrite(RedPin, HIGH);
  } else {
    digitalWrite(GrePin, LOW);
    digitalWrite(YelPin, LOW);
    digitalWrite(RedPin, LOW);
  }
}

void count(){
  time = millis();
}
