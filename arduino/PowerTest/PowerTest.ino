const int chipSelect = 10;
const int sdEnable = 7;

int s1 = 2;
int s2 = 8;

int led1 = 3;
int led2 = 6;
int led3 = 9;

#include <SdFat.h>
SdFat sd;
SdFile myFile;

void setup() {
  pinMode(sdEnable, OUTPUT);
  
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  
  pinMode(s1, INPUT_PULLUP);
  pinMode(s2, INPUT_PULLUP);

  //digitalWrite(sdEnable, HIGH);

}

int test = 5;

void loop() {
  digitalWrite(led1, HIGH);
  while(digitalRead(s1)){}
  while(!digitalRead(s1)){}
  digitalWrite(led3, HIGH);
  delay(1000);

  digitalWrite(sdEnable, HIGH);
  delay(500);
  
  if (!sd.begin(chipSelect, SPI_HALF_SPEED)) sd.initErrorHalt();

  if (!myFile.open("test.txt", O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("opening test.txt for write failed");
  }
  myFile.println("Test ");
  myFile.close();
  
  delay(200);
  digitalWrite(led3, LOW);
  digitalWrite(sdEnable, LOW);
  digitalWrite(led1, LOW);

  delay(5000);

  digitalWrite(led2, HIGH);
  while(digitalRead(s2)){}
  while(!digitalRead(s2)){}
  digitalWrite(led3, HIGH);
  delay(1000);

  digitalWrite(led3, LOW);
  digitalWrite(sdEnable, LOW);
  digitalWrite(led2, LOW);
  delay(5000);

  digitalWrite(led3, HIGH);
  delay(2000);
  digitalWrite(led3, LOW);
}

