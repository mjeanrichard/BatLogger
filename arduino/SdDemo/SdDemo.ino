
const int chipSelect = 10;
const int sdEnable = 7;

int s1 = 2;
int s2 = 8;

#include <SdFat.h>
SdFat sd;
SdFile myFile;

void setup() {
  Serial.begin(57600);

  pinMode(sdEnable, OUTPUT);
  pinMode(s1, INPUT_PULLUP);
  pinMode(s2, INPUT_PULLUP);

  //digitalWrite(sdEnable, HIGH);

}

int test = 5;

void loop() {
  Serial.println("Press S1 for write...");
  while(digitalRead(s1)){}
  while(!digitalRead(s1)){}

  delay(1000);
  Serial.println("Writing...");

  digitalWrite(sdEnable, HIGH);
  delay(500);
  
  if (!sd.begin(chipSelect, SPI_HALF_SPEED)) sd.initErrorHalt();

  if (!myFile.open("test.txt", O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("opening test.txt for write failed");
  }
  
  Serial.println("Writing to test.txt...");
  myFile.print("Test ");
  myFile.println(test++);

  // close the file:
  myFile.close();
  
  delay(200);
  digitalWrite(sdEnable, LOW);
  Serial.println("Press S2 for read...");

  while(digitalRead(s2)){}
  while(!digitalRead(s2)){}

  delay(1000);
  Serial.println("Reading...");
  
  digitalWrite(sdEnable, HIGH);
  delay(200);

  if (!sd.begin(chipSelect, SPI_HALF_SPEED)) sd.initErrorHalt();

  // re-open the file for reading:
  if (!myFile.open("test.txt", O_READ)) {
    sd.errorHalt("opening test.txt for read failed");
  }
  Serial.println("test.txt:");

  // read from the file until there's nothing else in it:
  int data;
  while ((data = myFile.read()) >= 0) Serial.write(data);
  // close the file:
  myFile.close();

  delay(200);
  digitalWrite(sdEnable, LOW);
  Serial.println("done.");

}


