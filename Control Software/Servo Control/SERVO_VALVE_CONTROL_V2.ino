//  Uses Arduino Nano, PCA9685 in Servo mode DWR Grant
// July 15, 2022
// Last Edited September 20, 2022 by A. Correa 
// W. K. Mizuno
// Kim K. Kardashian
// A. Correa
//  Active low puttons
//  Hook up LED's to PWM outputs to adjust brightness
 
/* Include the HCPCA9685 library */
#include "HCPCA9685.h"

/* I2C slave address for the device/module. For the HCMODU0097 the default I2C address
   is 0x40 */
#define  I2CAdd 0x40


/* Create an instance of the library */
HCPCA9685 HCPCA9685(I2CAdd);

const int SRVBUTN = 2;        //Service button color = green
const int RNSBUTN = 4;        //Rinse button color = blue
const int REGBUTN = 7;        //Regenerate button color = yellow
const int BKWBUTN = 8;        //Backwash button color = red
const int SHFTTGL = 11;      //Toggle between Transient/SteadyState
const int BKWINTERBUTN = 12;   //Backwash interlock button = pb red
float PURGEBUTN1 = A7 ;     // DI Purge button to expand resin = black
float PURGEBUTN2 = A2 ;     // Purge Button 2



const int SRVLED = 3;         //Service LED = green
const int RNSLED = 5;         //Rinse LED = blue
const int REGLED = 6;         //Regenerate LED = yellow
const int BKWLED = 9;         //Backwash LED = red
const int SHFTLED = 10;       //SHIFT LED = flashing white
const int PURGEBUTNLED1 = A6 ;    // LED to indicate purge style 1
const int PURGEBUTNLED2 = A1 ;    // LED to indicate purge style 2

const int VALVE1 = 1;
const int VALVE2 = 2;
const int VALVE3 = 3;
const int VALVE4 = 4;
const int VALVE5 = 5;
const int VALVE6 = 6;
const int VALVE7 = 7;
const int VALVE8 = 8;
const int VALVE9 = 9;
const int VALVE10 = 10;     //Added below the IX column 09.27.2022




unsigned int APOS = 355;         //Valve A position
unsigned int BPOS = -50;        //Valve B position
const int ValveStagger = 1500;     //Delay between valve actuation to lessen current draw
int LatchSRV = HIGH;
int LatchRNS = HIGH;
int LatchREG = HIGH;
int LatchBKW = HIGH;
int LatchSHFT = HIGH;
int LatchBKWINT = HIGH;
int LatchPURGE = HIGH;
int ButtonPushSRV = HIGH;
int ButtonPushRNS = HIGH;
int ButtonPushREG = HIGH;
int ButtonPushBKW = HIGH;
int ToggleTransient = HIGH;
float ButtonPushPURGE1 = HIGH;
int ButtonPushPURGE2 = HIGH;


void setup() {
  Serial.begin(9600);
  /* Initialise the library and set it to 'servo mode' */
  HCPCA9685.Init(SERVO_MODE);

  /* Wake the device up */
  HCPCA9685.Sleep(false);

  pinMode (SRVBUTN, INPUT);
  pinMode (RNSBUTN, INPUT);
  pinMode (REGBUTN, INPUT);
  pinMode (BKWBUTN, INPUT);
  pinMode (SHFTTGL, INPUT);
  pinMode (BKWINTERBUTN, INPUT);
  pinMode (PURGEBUTN1, INPUT);
  pinMode (PURGEBUTN2, INPUT);

  pinMode (SRVLED, OUTPUT);
  pinMode (RNSLED, OUTPUT);
  pinMode (REGLED, OUTPUT);
  pinMode (BKWLED, OUTPUT);
  pinMode (SHFTLED, OUTPUT);
  pinMode (PURGEBUTNLED1, OUTPUT);
  pinMode (PURGEBUTNLED2, OUTPUT);
}

void loop() {

Beginning:  
  LatchSHFT = digitalRead(SHFTTGL);
  if (LatchSHFT == HIGH) {
    digitalWrite(SHFTLED, HIGH);
    Serial.println("I am toggled white light");
  }
  else {
    digitalWrite(SHFTLED, LOW);
  }
LatchSHFT = digitalRead(SHFTTGL);

  ButtonPushSRV = digitalRead(SRVBUTN);
  if (ButtonPushSRV == LOW) {
    LatchSRV = LOW;
    LatchRNS = HIGH;
    LatchREG = HIGH;
    LatchBKW = HIGH;
    LatchPURGE = HIGH;
    Serial.println("ServiceButton Ground");
    goto ServiceSteady;
  }
  else {
    LatchSRV = HIGH;
  }

  ButtonPushRNS = digitalRead(RNSBUTN);
  if (ButtonPushRNS == LOW) {
    LatchSRV = HIGH;
    LatchRNS = LOW;
    LatchREG = HIGH;
    LatchBKW = HIGH;
    LatchPURGE = HIGH;
  }
  else {
    LatchRNS = HIGH;
  }

  ButtonPushREG = digitalRead(REGBUTN);
  if (ButtonPushREG == LOW) {
    LatchSRV = HIGH;
    LatchRNS = HIGH;
    LatchREG = LOW;
    LatchBKW = HIGH;
    LatchPURGE = HIGH;
  }
  else {
    LatchREG = HIGH;
  }

  ButtonPushBKW = digitalRead(BKWBUTN);
  if (ButtonPushBKW == LOW) {
    LatchSRV = HIGH;
    LatchRNS = HIGH;
    LatchREG = HIGH;
    LatchBKW = LOW;
    LatchPURGE = HIGH;

  }
  else {
    LatchBKW = HIGH;

  }
 
  // STEADY STATE MODES*************************************************************************

  //SERVICE MODE Steady State*********************************************************************
  if ((LatchSRV == LOW) && (LatchSHFT == HIGH)) {
    digitalWrite(SRVLED, HIGH);   //Turn off LED's and turn on the Service LED only
    digitalWrite(RNSLED, LOW);
    digitalWrite(REGLED, LOW);
    digitalWrite(BKWLED, LOW);
    digitalWrite(SHFTLED, LOW);
    LatchRNS = HIGH;
    LatchREG = HIGH;
    LatchBKW = HIGH;
    LatchPURGE = HIGH;
    Serial.println("Checking Conditions");
    goto ServiceSteady;
  }
  else {
    LatchSRV = HIGH;
  }


  //RINSE MODE Steady State*********************************************************************
  if ((LatchRNS == LOW) && (LatchSHFT == HIGH)) {
    digitalWrite(SRVLED, LOW);   //Turn off LED's and turn on the Rinse LED only
    digitalWrite(RNSLED, HIGH);
    digitalWrite(REGLED, LOW);
    digitalWrite(BKWLED, LOW);
    digitalWrite(SHFTLED, LOW);
    LatchSRV = HIGH;
    LatchREG = HIGH;
    LatchBKW = HIGH;
    LatchPURGE = HIGH;
    goto RinseSteady;
  }
  else {
    LatchRNS = HIGH;
  }


  //REGENERATION MODE Steady State*********************************************************************
  if ((LatchREG == LOW) && (LatchSHFT == HIGH)) {
    digitalWrite(SRVLED, LOW);   //Turn off LED's and turn on the Regen LED only
    digitalWrite(RNSLED, LOW);
    digitalWrite(REGLED, HIGH);
    digitalWrite(BKWLED, LOW);
    digitalWrite(SHFTLED, LOW);
    LatchSRV = HIGH;
    LatchRNS = HIGH;
    LatchBKW = HIGH;
    LatchPURGE = HIGH;
    goto RegenSteady;
  }
  else {
    LatchREG = HIGH;
  }


  //BACKWASH MODE Steady State*********************************************************************
  LatchBKWINT = digitalRead(BKWINTERBUTN);
  if ((LatchBKW == LOW) && ((LatchSHFT == HIGH) ||(LatchSHFT == LOW))) {
    digitalWrite(SRVLED, LOW);   //Turn off LED's and turn on the Backwash LED only
    digitalWrite(RNSLED, LOW);
    digitalWrite(REGLED, LOW);
    digitalWrite(BKWLED, HIGH);
    digitalWrite(SHFTLED, LOW);
    LatchSRV = HIGH;
    LatchRNS = HIGH;
    LatchREG = HIGH;
    LatchPURGE = HIGH;
    goto BackwashSteady;
  }
  else {
    LatchBKW = HIGH;
  }


  // TRANSIENT MODES*************************************************************************

//  LatchSHFT = digitalRead(SHFTTGL);
//  if (LatchSHFT == HIGH) {
//    digitalWrite(SHFTLED, HIGH);
//  }
//  else {
//    digitalWrite(SHFTLED, LOW);
//  }

  ButtonPushSRV = digitalRead(SRVBUTN);
  if (ButtonPushSRV == LOW) {
    LatchSRV = LOW;
    LatchRNS = HIGH;
    LatchREG = HIGH;
    LatchBKW = HIGH;
    LatchPURGE = HIGH;
  }
  else {
    LatchSRV = HIGH;
  }

  ButtonPushRNS = digitalRead(RNSBUTN);
  if (ButtonPushRNS == LOW) {
    LatchSRV = HIGH;
    LatchRNS = LOW;
    LatchREG = HIGH;
    LatchBKW = HIGH;
    LatchPURGE = HIGH;
  }
  else {
    LatchRNS = HIGH;
  }

  ButtonPushREG = digitalRead(REGBUTN);
  if (ButtonPushREG == LOW) {
    LatchSRV = HIGH;
    LatchRNS = HIGH;
    LatchREG = LOW;
    LatchBKW = HIGH;
    LatchPURGE = HIGH;
  }
  else {
    LatchREG = HIGH;
  }

  ButtonPushBKW = digitalRead(BKWBUTN);
  if (ButtonPushBKW == LOW) {
    LatchSRV = HIGH;
    LatchRNS = HIGH;
    LatchREG = HIGH;
    LatchBKW = LOW;
    LatchPURGE = HIGH;

  }
  else {
    LatchBKW = HIGH;

  }



  //SERVICE MODE Transient******************************************************************
  if ((LatchSRV == LOW)  && (LatchSHFT == LOW))  {
    digitalWrite(SRVLED, HIGH);   //Turn off LED's and turn on the Service LED only
    digitalWrite(RNSLED, LOW);
    digitalWrite(REGLED, LOW);
    digitalWrite(BKWLED, LOW);
    digitalWrite(SHFTLED, LOW);
    LatchRNS = HIGH;
    LatchREG = HIGH;
    LatchBKW = HIGH;
     LatchPURGE = HIGH;
    goto ServiceTransient;
  }
  else {
    LatchSRV = HIGH;
  }

  //RINSE MODE Transient***********************************************************************
  if ((LatchRNS == LOW) && (LatchSHFT == LOW)) {
    digitalWrite(SRVLED, LOW);   //Turn off LED's and turn on the Rinse LED only
    digitalWrite(RNSLED, HIGH);
    digitalWrite(REGLED, LOW);
    digitalWrite(BKWLED, LOW);
    digitalWrite(SHFTLED, LOW);
    LatchSRV = HIGH;
    LatchREG = HIGH;
    LatchBKW = HIGH;
     LatchPURGE = HIGH;
    goto RinseTransient;
  }
  else {
    LatchRNS = HIGH;
  }

  //REGENERATION MODE Transient*****************************************************************
  if ((LatchREG == LOW) && (LatchSHFT == LOW)) {
    digitalWrite(SRVLED, LOW);   //Turn off LED's and turn on the Regen LED only
    digitalWrite(RNSLED, LOW);
    digitalWrite(REGLED, HIGH);
    digitalWrite(BKWLED, LOW);
    digitalWrite(SHFTLED, LOW);
    LatchSRV = HIGH;
    LatchRNS = HIGH;
    LatchBKW = HIGH;
     LatchPURGE = HIGH;
    goto RegenTransient;
    
  }
  else {
    LatchREG = HIGH;
  }

  //BACKWASH MODE Transient***********************************************************************
  LatchBKWINT = digitalRead(BKWINTERBUTN);
  if ((LatchBKW == LOW) && (LatchBKWINT == LOW) && (LatchSHFT == LOW)) {
    digitalWrite(SRVLED, LOW);   //Turn off LED's and turn on the Backwash LED only
    digitalWrite(RNSLED, LOW);
    digitalWrite(REGLED, LOW);
    digitalWrite(BKWLED, HIGH);
    digitalWrite(SHFTLED, LOW);
    LatchSRV = HIGH;
    LatchRNS = HIGH;
    LatchREG = HIGH;
     LatchPURGE = HIGH;
    goto BackwashTransient;
  }
  else {
    LatchBKW = HIGH;
  }

goto Beginning;

//DI_PURGE: 
//    { 
//
//  digitalWrite(A6, HIGH);
//  HCPCA9685.Servo(VALVE1, APOS);
//  delay(ValveStagger);
//  HCPCA9685.Servo(VALVE2, BPOS);
//  delay(ValveStagger);
//  HCPCA9685.Servo(VALVE3, BPOS);
//  delay(ValveStagger);
//  HCPCA9685.Servo(VALVE4, APOS);
//  delay(ValveStagger);
//  HCPCA9685.Servo(VALVE5, BPOS);
//  delay(ValveStagger);
//  HCPCA9685.Servo(VALVE6, APOS);
//  delay(ValveStagger);
//  HCPCA9685.Servo(VALVE7, BPOS);
//  delay(ValveStagger);
//  HCPCA9685.Servo(VALVE8, APOS);
//  delay(ValveStagger);
//  HCPCA9685.Servo(VALVE9, BPOS);
//  delay(ValveStagger);
//  HCPCA9685.Servo(VALVE10, BPOS);
//  delay(ValveStagger);
//}

goto Beginning;

  //SWITCH POSITIONS

  //SERVICE MODE Steady State*********************************************************************************
ServiceSteady:
 { HCPCA9685.Servo(VALVE1, BPOS);
  delay(ValveStagger);
  HCPCA9685.Servo(VALVE2, APOS);
  delay(ValveStagger);
  HCPCA9685.Servo(VALVE3, BPOS);
  delay(ValveStagger);
  HCPCA9685.Servo(VALVE4, APOS);
  delay(ValveStagger);
  HCPCA9685.Servo(VALVE5, BPOS);
  delay(ValveStagger);
  HCPCA9685.Servo(VALVE6, APOS);
  delay(ValveStagger);
  HCPCA9685.Servo(VALVE7, BPOS);
  delay(ValveStagger);
  HCPCA9685.Servo(VALVE8, APOS);
  delay(ValveStagger);
  HCPCA9685.Servo(VALVE9, APOS);
  delay(ValveStagger);
  HCPCA9685.Servo(VALVE10, BPOS);
  delay(ValveStagger);
}
goto Beginning;
//RINSE MODE Steady State*********************************************************************************
RinseSteady:
/* if ((digitalRead(RNSBUTN) == LOW) && (digitalRead(SHFTBUTN == HIGH))) {

  digitalWrite(SRVLED, LOW);
  digitalWrite(RNSLED, HIGH);    //Turn off LED's and turn on the Service LED only
  digitalWrite(REGLED, LOW);
  digitalWrite(BKWLED, LOW);
*/
{
HCPCA9685.Servo(VALVE1, APOS);  //Reset to APOS
delay(ValveStagger);
HCPCA9685.Servo(VALVE2, APOS);  
delay(ValveStagger);
HCPCA9685.Servo(VALVE3, BPOS);  
delay(ValveStagger);
HCPCA9685.Servo(VALVE4, APOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE5, APOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE6, APOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE7, APOS); // Valve 6 in real life 03/25/23 - Armando 
delay(ValveStagger);
HCPCA9685.Servo(VALVE8, BPOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE9, BPOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE10, BPOS);
delay(ValveStagger);
}
goto Beginning;

//REGENERATION MODE Steady State*********************************************************************************
RegenSteady:
/* if ((digitalRead(REGBUTN) == LOW) && (digitalRead(SHFTBUTN == HIGH))) {

  digitalWrite(SRVLED, LOW);
  digitalWrite(RNSLED, LOW);    //Turn off LED's and turn on the Service LED only
  digitalWrite(REGLED, HIGH);
  digitalWrite(BKWLED, LOW);

*/
{
HCPCA9685.Servo(VALVE1, APOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE2, APOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE3, BPOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE4, APOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE5, APOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE6, APOS); // Valve 4 in Real Life
delay(ValveStagger);
HCPCA9685.Servo(VALVE7, BPOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE8, APOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE9, APOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE10, BPOS);
delay(ValveStagger);
}
goto Beginning;
//BACKWASH MODE Steady State*********************************************************************************
BackwashSteady:
{
HCPCA9685.Servo(VALVE1, APOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE2, APOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE3, APOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE4, APOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE5, APOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE6, APOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE7, APOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE8, APOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE9, APOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE10, APOS);
delay(ValveStagger);
}
goto Beginning;

//TRANSIENT STATE CONDITION*********************************************************************



//SERVICE MODE Transient*********************************************************************
ServiceTransient:
/* if ((digitalRead(SRVBUTN) == LOW) && (digitalRead(SHFTBUTN == LOW))) {

  digitalWrite(SRVLED, HIGH);   //Turn off LED's and turn on the Service LED only
  digitalWrite(RNSLED, LOW);
  digitalWrite(REGLED, LOW);
  digitalWrite(BKWLED, LOW);
  digitalWrite (SHFTLED, HIGH);
  delay(500);
  digitalWrite (SHFTLED, LOW);
  delay(500);
  digitalWrite (SRVLED, HIGH);
  delay(500);
  digitalWrite (SRVLED, LOW);
  delay(500);

*/
{
HCPCA9685.Servo(VALVE1, BPOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE2, APOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE3, BPOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE4, APOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE5, BPOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE6, BPOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE7, BPOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE8, APOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE9, BPOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE10, BPOS);
delay(ValveStagger);
}
goto Beginning;

//RINSE MODE Transient*********************************************************************************
RinseTransient:
/*if ((digitalRead(RNSBUTN) == LOW) && (digitalRead(SHFTBUTN == LOW))) {

  digitalWrite(SRVLED, LOW);
  digitalWrite(RNSLED, HIGH);    //Turn off LED's and turn on the Service LED only
  digitalWrite(REGLED, LOW);
  digitalWrite(BKWLED, LOW);
  digitalWrite (SHFTLED, HIGH);
  delay(500);
  digitalWrite (SHFTLED, LOW);
  delay(500);
  digitalWrite (RNSLED, HIGH);
  delay(500);
  digitalWrite (RNSLED, LOW);
  delay(500);
*/
{
  HCPCA9685.Servo(VALVE1, APOS);
  delay(ValveStagger);
  HCPCA9685.Servo(VALVE2, APOS);
  delay(ValveStagger);
  HCPCA9685.Servo(VALVE3, BPOS);
  delay(ValveStagger);
  HCPCA9685.Servo(VALVE4, BPOS);
  delay(ValveStagger);
  HCPCA9685.Servo(VALVE5, BPOS);
  delay(ValveStagger);
  HCPCA9685.Servo(VALVE6, BPOS);
  delay(ValveStagger);
  HCPCA9685.Servo(VALVE7, BPOS);
  delay(ValveStagger);
  HCPCA9685.Servo(VALVE8, BPOS);
  delay(ValveStagger);
  HCPCA9685.Servo(VALVE9, BPOS);
  delay(ValveStagger);
  HCPCA9685.Servo(VALVE10, BPOS);
delay(ValveStagger);
}
goto Beginning;
//REGENERATION MODE Transient*********************************************************************************
RegenTransient:
/*if ((digitalRead(REGBUTN) == LOW) && (digitalRead(SHFTBUTN == LOW))) {

  digitalWrite(SRVLED, LOW);
  digitalWrite(RNSLED, LOW);    //Turn off LED's and turn on the Service LED only
  digitalWrite(REGLED, HIGH);
  digitalWrite(BKWLED, LOW);
  digitalWrite (SHFTLED, HIGH);
  delay(500);
  digitalWrite (SHFTLED, LOW);
  delay(500);
  digitalWrite (REGLED, HIGH);
  delay(500);
  digitalWrite (REGLED, LOW);
  delay(500);

*/
{
HCPCA9685.Servo(VALVE1, APOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE2, APOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE3, BPOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE4, BPOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE5, BPOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE6, BPOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE7, BPOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE8, APOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE9, BPOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE10, BPOS);
delay(ValveStagger);
}
goto Beginning;
//BACKWASH MODE Transient*********************************************************************************
BackwashTransient:
/* if ((digitalRead(SRVBUTN) == LOW) && (digitalRead(BKWINTERBUTN) == LOW) && (digitalRead(SHFTBUTN == LOW)) {

  digitalWrite(SRVLED, LOW);
  digitalWrite(RNSLED, LOW);    //Turn off LED's and turn on the Service LED only
  digitalWrite(REGLED, LOW);
  digitalWrite(BKWLED, HIGH);
  digitalWrite (SHFTLED, HIGH);
  delay(500);
  digitalWrite (SHFTLED, LOW);
  delay(500);
  digitalWrite (BKWLED, HIGH);
  delay(500);
  digitalWrite (BKWLED, LOW);
  delay(500);

*/
{
HCPCA9685.Servo(VALVE1, APOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE2, BPOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE3, BPOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE4, APOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE5, APOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE6, APOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE7, APOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE8, BPOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE9, BPOS);
delay(ValveStagger);
HCPCA9685.Servo(VALVE10, APOS);
delay(ValveStagger);
}

goto Beginning;
}
