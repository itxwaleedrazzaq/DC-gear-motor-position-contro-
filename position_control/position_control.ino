#include <L298N.h>   // motordriver library
#include <EEPROM.h>  //eeprom library

//first check the Rpms using RPM_checker
int   primaryShaftRpm = 6000;                 // put values according to applied voltages
int   secondaryShaftRpm = 150;                  //secondry shaft RPM

float  shaft_ratio;
long   angle_ratio;  
int  user_angle;
long  sub =0;
long  print_sub;
long  shaftRotation = 0;
volatile long  storeRotation = 0;

int   ENA = 9; 
int   IN1 = 8;           //motor driver pins
int   IN2 = 7;

L298N myBridge(ENA,IN1,IN2);   //object of l298n

int16_t reboot_val;   // reboot value
int16_t prev_shaft;     //reset the position 
int16_t prev_store;

void setup()
{
  Serial.begin(9600);           //Serial monitor initilaized
  pinMode(ENA,OUTPUT);           
  pinMode(IN1,OUTPUT);          //..MOtor driver pins
  pinMode(IN2,OUTPUT);
  shaft_ratio = ((360.*secondaryShaftRpm)/primaryShaftRpm);   // this is for resolution of motor to achieve
  angle_ratio = round(shaft_ratio);         //rounding off to get non float value

  // first value will be restored

  prev_shaft = (int16_t)EEPROM.read(1);    // previus shaft was saves at address 1 to restore further
  delay(30);
  prev_store = (int16_t)EEPROM.read(2);    // previus shaft was saves at address 2 to restore further
  delay(30);

  reboot_val = prev_shaft + prev_store;           // will be the sum to 
  
  //Serial.println(reboot_val);

  if( reboot_val < 0)                     //if reboot_value is less than zero the.
  {

    shaftRotation = reboot_val;             //reboot_value is stored to shaftRotation for the loop
    digitalWrite(ENA,HIGH);                 //Motor will move clockwsie
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,HIGH);
    attachInterrupt(0,Interrupt2,RISING);      //Interrupt is attached a function Interrupt 2
    EEPROM.write(0,1);                         //after using values reset the adresse
    EEPROM.write(0,2);
   }

  if( reboot_val > 0)                  //if reboor_valu is greater then zero
  { 
    shaftRotation = reboot_val;         // put value in shaftRotation to start the loop
    digitalWrite(ENA,HIGH);
    digitalWrite(IN1,LOW);             //motor driver will run anticlockwise
    digitalWrite(IN2,HIGH);
    attachInterrupt(0,InterruptFunction,RISING);        //Interrupt will attach at function InterruptFunction
    EEPROM.write(1,0);
    EEPROM.write(2,0);                           // reseting value after using that are stored in EEPROm
  }
  }

void loop()

{
   digitalWrite(ENA,HIGH);           //You should analogWrite(pin,value) to variate the motor speed according to your ned
  
  Serial.println("-- Enter value between -360 and 360--");      //prompt the user to send the desired angle
  while(Serial.available()==0){}               //wait until the user send a value
  user_angle = Serial.parseInt();                //enter the value to variable user_angle
  
  
  shaftRotation = user_angle * angle_ratio;        //ShaftRotation will multiply with angle_ratio to get the desried counter for loop
  shaftRotation = 2 * shaftRotation;               // rotater has 20 holes
  EEPROM.write(1,shaftRotation);                   //saving shaftRotation to EEPROM
 
  sub = sub - storeRotation;                     // making difference for entering zero condition     
  print_sub = sub/(2*angle_ratio);               //making sub to a user friendly variable
 
  if(print_sub <= -360)                         // if print_sub is less than -360
  {
    print_sub = print_sub + 360;                // for not rotation an extra 360 angle
  }

  if(print_sub >= 360)                         //if print_sub is greater than 360
  {
    print_sub = print_sub -360;                // for not rotation an extra 360 angle
  }

 if(user_angle == 0)                                           // if user enter zero then it makes 3 cases
 {
  if(sub < 0)                                                 // if previos porition of shaft is negative   
  {
      if(sub <= -360)
      {
        shaftRotation = print_sub*2*angle_ratio;     //then put print_sub into shaftRotation for loop
      }
      else
      {
      shaftRotation = sub;                       // put sub for loop
      }
      Serial.print(" Rotation of Secondary shaft angle is : ");      //printef at serialMonitor
      Serial.println(print_sub);
      digitalWrite(ENA,HIGH);
      digitalWrite(IN1,LOW);        // move clockwise
      digitalWrite(IN2,HIGH);
      attachInterrupt(0,Interrupt2,RISING);
  }
  if(sub > 0)                                             // if previous position of shaft is poitive
  {
      if(sub > 360)
      {
        shaftRotation = print_sub*2*angle_ratio;        //then put print_sub into shaftRotation for loop
      }
      else
      {
      shaftRotation = sub;                     // put sub for loop
      }
      Serial.print(" Rotation of Secondary shaft angle is : ");
      Serial.println(print_sub);
      digitalWrite(ENA,HIGH);
      digitalWrite(IN1,HIGH);       //will move anticlockwise
      digitalWrite(IN2,LOW);
    attachInterrupt(0,InterruptFunction,RISING);
  }
  if(sub  == 0)                                         // if previous position is zero
  {
     Serial.println(" No rotation");
    digitalWrite(ENA,LOW);
    digitalWrite(IN1,LOW);                      // no rotation
    digitalWrite(IN2,LOW);
  }
 }
 
 if(user_angle > 0 & user_angle <=360)                 // enter user value between 0 and 360
 {
    Serial.print(" Rotation of Secondary shaft angle is : ");
    Serial.println(shaftRotation/(2*angle_ratio));
    digitalWrite(ENA,HIGH);
    digitalWrite(IN1,HIGH);        // move clockwise
    digitalWrite(IN2,LOW);
    attachInterrupt(0,InterruptFunction,RISING);
 }
 if(user_angle >= -360 & user_angle < 0)     // user enter value between -360 to 0
 {
    Serial.print(" Rotation of Secondary shaft angle is : ");
    Serial.println(shaftRotation/(2*angle_ratio));        // justing printing the angle
    digitalWrite(ENA,HIGH);
    digitalWrite(IN1,LOW);        // move anticlockwise
    digitalWrite(IN2,HIGH);
    attachInterrupt(0,Interrupt2,RISING);  //interrupt will atatch to function 2
 }

   storeRotation = shaftRotation;                    // previous shafRotation value is saved value is stored
   EEPROM.write(2,storeRotation);                    //value is store in memory
   delay(200);
   Serial.println("");
   Serial.println("");
 }

 void InterruptFunction()                                 // clockwise rotation
{
  detachInterrupt(0);                                    //Interrupt is detached   
  shaftRotation--;                                      //decremnt in shaftRotation value
  attachInterrupt(0,InterruptFunction,RISING);          //interrupt  is acctched
  if(shaftRotation == 0)
  {
    digitalWrite(ENA,LOW);
    digitalWrite(IN1,LOW);        // stoping the driver
    digitalWrite(IN2,LOW);
  }
}

void Interrupt2()                // anti clockwise rotation
{
  shaftRotation = abs(shaftRotation);     //shaftRotation = -shaftRotation for loo[
  detachInterrupt(0);                      //detachInterrupt
  shaftRotation--;
  attachInterrupt(0,Interrupt2,RISING);
  if(shaftRotation == 0)
  {
    digitalWrite(ENA,LOW);
    digitalWrite(IN1,LOW);        // move clockwise
    digitalWrite(IN2,LOW);
  }
}
