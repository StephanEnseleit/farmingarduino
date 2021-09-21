#include <Wire.h>
#include <string.h>

byte slave_address = 7;

void setup() {
  Wire.begin(slave_address);
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);
}

void loop() {
}

void receiveEvent(int howMany) {
  
  int numOfBytes = Wire.available();
 
  byte b = Wire.read();  //cmd
  Serial.println(b);
  char str[10] = "";
  for(int i=0; i<numOfBytes-1; i++){
    char data = Wire.read();
    strncat(str,&data,1);
  }
  Serial.println(str);  
}
