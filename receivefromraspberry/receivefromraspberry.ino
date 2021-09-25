#include <Wire.h>
#include <string.h>
#include <AccelStepper.h>
#include <MultiStepper.h>

byte slave_address = 7;
long positions[2];
int counter = 0;
AccelStepper stepper1(AccelStepper::FULL4WIRE, 2, 3, 4, 5); //degree motor
AccelStepper stepper2(AccelStepper::FULL4WIRE, 8, 9, 10, 11); //radius motor

void setup() {
  Wire.begin(slave_address);
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);

  stepper1.setMaxSpeed(100);
  stepper2.setMaxSpeed(100);
  stepper1.setAcceleration(20);
  stepper2.setAcceleration(20);
}

void loop() {
  if (counter % 2 == 0) {

      //probably needs to add some factor, maybe steps for one whole rotation
      
      stepper1.moveTo(positions[0]);
      stepper2.moveTo(positions[1]);
    }
}

void receiveEvent(int howMany) {
  counter += 1;
  int numOfBytes = Wire.available();
   
  byte b = Wire.read();  //cmd
  
  char str[8] = "";
  for(int i=0; i<numOfBytes-1; i++){
    char data = Wire.read();
    strncat(str,&data,1);
  }
  if (b == 0) { //degree
    positions[0] = atoi(str);
    }
  if (b == 1) { //radius
    positions[1] = atoi(str);
    }
  Serial.println(counter);
}
