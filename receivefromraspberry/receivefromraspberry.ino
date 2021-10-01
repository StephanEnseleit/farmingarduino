#include <Wire.h>
#include <string.h>
#include <AccelStepper.h>
#include <EnableInterrupt.h> 

#define encoderA 5
#define encoderB 6
#define encoderZ 7

byte slave_address = 7;
long positions[2];
int counter = 0;

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
AccelStepper radius(1, 5, 6); //radius motor

void setup() {
  Wire.begin(slave_address);
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);  

  rotor.setMaxSpeed(30000);
 // stepper2.setMaxSpeed(100);
  rotor.setAcceleration(1500);
 // stepper2.setAcceleration(20);

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
  rotor.moveTo(angle); //set distance
      while (rotor.distanceToGo() > 0) {
          rotor.run();
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
    }
  if (b == 1) { //radius
    positions[1] = atoi(str);
    }
  Serial.println(positions[0]);
  moveRotor(positions[0]);
}

/*
 * ***** Rotary Encoder Functions *****
 */

void checkDirection(){
  if(digitalRead(encoderB) ==  HIGH){                             //digitalReadFast() is faster than digitalRead()
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
