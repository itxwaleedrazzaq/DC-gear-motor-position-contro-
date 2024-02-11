#include <L298N.h>
#include <TimerOne.h>

int EN1 = 9;
int IN1 = 8;
int IN2 = 7;
int rpm;
unsigned int counter;

L298N mymotor(EN1, IN1, IN2);  // motor object


void setup() {
  Serial.begin(9600);  // enable serial monitor
  pinMode(EN1,OUTPUT);
  pinMode(IN1,OUTPUT);
  pinMode(IN1,OUTPUT);         // Setting pins

  Timer1.initialize(1000000);
  attachInterrupt(0,count,RISING);
  Timer1.attachInterrupt(timemanager);
  }
  

void loop() 
{
  
  digitalWrite(EN1,HIGH);       // clockwise rotation
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);
  
}

void timemanager()
{
  Timer1.detachInterrupt();
  Serial.print(" The RPM of primary shaft is: ");
  rpm = counter/20;
  rpm = rpm * 60;
  Serial.println(rpm);
  Serial.println(" The RPM of secondary shaft is :");
  counter = 0;
  Timer1.attachInterrupt(timemanager);
  
}
void count()
{
  counter++; // count it
}

 
