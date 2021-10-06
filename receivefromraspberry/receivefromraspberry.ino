#include <Wire.h>
#include <string.h>
#include <AccelStepper.h>
#include <EnableInterrupt.h> 
#include <stdio.h>
#include <stdlib.h>

#define encoderA 5
#define encoderB 6
#define encoderZ 7

byte slave_address = 7;
long positions[2];
int counter = 0;
float encoderFactor = 2.7777;

volatile int countA = 0;
volatile int countB = 0;
volatile int cumulativeCountA = 0;
volatile int cumulativeCountB = 0;
int pulsesPerRev = 1000;     //This variable will is unique to the type of encoder
int Dir = 0;  // 1 = CW
              // 0 = Stationary
              // -1 = CCW
              
// Pulses D3, direction D4, Enable D5
AccelStepper rotor(1, 3, 4); //degree motor
AccelStepper radius(1, 8, 9); //radius motor
AccelStepper height(1, 10, 11); //height motor

void setup() {
  Wire.begin(slave_address);
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);  

  rotor.setMaxSpeed(30000);
  radius.setMaxSpeed(30000);
  rotor.setAcceleration(1500);
  radius.setAcceleration(1500);
  height.setMaxSpeed(30000);
  height.setAcceleration(1500);

  pinMode(encoderA, INPUT_PULLUP);
  pinMode(encoderB, INPUT_PULLUP);
  pinMode(encoderZ, INPUT_PULLUP);  
  enableInterrupt(encoderA, pulseA, RISING);
  enableInterrupt(encoderB, pulseB, RISING);
  enableInterrupt(encoderZ, pulseZ, RISING);
}

void loop() {
  Serial.print(countA);
  Serial.print('\t');
  Serial.print(countB);
  Serial.print('\t');
  Serial.print(cumulativeCountA);
  Serial.print('\t');
  Serial.print(cumulativeCountB);
  Serial.print('\t');
  Serial.println(Dir); 
}


/**
 * moves bot in angle direction
 */
void moveRotor(int angle) {
  cumulativeCountA = 0;
  rotor.moveTo(angle); //set distance
      while (angle - (abs(cumulativeCountA)/encoderFactor) > 0) {
          rotor.run();
      }
  }

void moveRadius(int r) {
    radius.moveTo(r);
    while (radius.distanceToGo() > 0) {
      radius.run();
      }
  }

void moveZ(int z) {
    height.moveTo(z);
    while (height.distanceToGo() > 0) {
      height.run();
      }
  }

/**
 * I2C Receive and make integer values
 */
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
    moveRotor(positions[0]);
    }
  if (b == 1) { //radius
    positions[1] = atoi(str);
    moveRadius(positions[1]);
    }
  if (b == 2) {
    positions[2] = atoi(str);
    moveZ(positions[2]);
    }
  Serial.println(positions[0]);
}

/*
 * ***** Rotary Encoder Functions *****
 */

void checkDirection(){
  if(digitalRead(encoderB) ==  HIGH){
    Dir = 1;  
  }
  else if(digitalRead(encoderB) ==  LOW){
    Dir = -1;
  }
}

void pulseA(){  
  checkDirection();
  countA += Dir;
  cumulativeCountA += Dir;
}

void pulseB(){  
  countB += Dir;
  cumulativeCountB += Dir;  
}

void pulseZ(){
  countA = 0;   //reset counters at "home" reference point
  countB = 0;
}
