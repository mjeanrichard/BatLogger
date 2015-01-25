
int led1 = 3;
int led2 = 6;
int led3 = 9;

int s1 = 2;
int s2 = 8;

void setup() {
  // put your setup code here, to run once:
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  
  pinMode(s1, INPUT_PULLUP);
  pinMode(s2, INPUT_PULLUP);
}

int l1 = HIGH, l2 = LOW, l3 = LOW;

void loop() {
  
  while(!digitalRead(s1))
  {
    l2 = HIGH;
    l1 = LOW;
  }
  while(!digitalRead(s2))
  {
    l3 = HIGH;
    l1 = LOW;
  }

  digitalWrite(led1, l1);
  digitalWrite(led2, l2);
  digitalWrite(led3, l3);
  
}
