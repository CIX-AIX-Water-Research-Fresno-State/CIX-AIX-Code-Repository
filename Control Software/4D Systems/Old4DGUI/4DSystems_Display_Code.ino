//Miguel Sanchez
//Last Edited: 9/11/23
//Code if for the 4D display tocuh screen that will control the servos as well as displaying multiple values such as
//Water hardness, Elapsed time, EC, cumulative ion exchange, and bed volumes

#include <genieArduino.h>
#include "HCPCA9685.h"

/* I2C slave address for the device/module. For the HCMODU0097 the default I2C address
   is 0x40 */
#define  I2CAdd 0x40

/* Create an instance of the library */
HCPCA9685 HCPCA9685(I2CAdd);
Genie genie;

#define RESETLINE 4 //set the reset pin to digital 4

//Sensor pin assignments


const int HNpin = A0;
const int ECpin = A1;
const int FloMetpin = A2;
const int Transducer = A3;

//global variables
float cumulativeflow = 0.0;
float previousbedvolume = 0.0;
unsigned long startTime = 0;
const unsigned long updateInterval = 15000;
unsigned long previousMillis = 0;
bool buttonStartStop = false;
int ButtonState0 = 0;

//valve values
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

int modeselect = 0;
int LatchSHFT = HIGH;
int LatchRNS = HIGH;
int LatchREG = HIGH;
int LatchSRV = HIGH;
int LatchPURGE = HIGH;
int LatchAUTO = HIGH;

int LatchMastPower = HIGH;

//Auto Mode indicator
bool AUTOMODETRANS = false;
bool AUTOMODESTEADY = false;




void setup() {
Serial.begin(115200);
genie.Begin(Serial);

  /* Initialise the library and set it to 'servo mode' */
  HCPCA9685.Init(SERVO_MODE);

  /* Wake the device up */
  HCPCA9685.Sleep(false);

pinMode(RESETLINE, 1);
digitalWrite(RESETLINE, 1);
delay(100);
digitalWrite(RESETLINE, 0);
delay(2000);

genie.AttachEventHandler(IXControls);

}

void loop() {

  genie.DoEvents(); // checks for new events from the touch screen

  if (buttonStartStop){
    unsigned long currentTime = millis(); // Get the current time
    unsigned long elapsedTime = currentTime - startTime; // Calculate elapsed time
    
    // Calculate hours, minutes, and seconds
    unsigned long hours = elapsedTime / 3600000; // 1 hour = 3600000 milliseconds
    elapsedTime %= 3600000;
    unsigned long minutes = elapsedTime / 60000; // 1 minute = 60000 milliseconds
    elapsedTime %= 60000;
    unsigned long seconds = elapsedTime / 1000; // 1 second = 1000 milliseconds

    

    if(currentTime - previousMillis >= updateInterval){
      float hardness = analogRead(HNpin); // analog in for hardnsss
      float ec = analogRead(ECpin); // analog in for ec
      float FloMetmV = analogRead(FloMetpin); // analog in for flow meter
      float TDucer = analogRead(Transducer);

      //float TDucerVoltage = TDucer / 204.6;
      float flowRate  = FloMetmV * 0.03;
      float bedVolume = (flowRate / (4 * 1000) / 530.1438) + previousbedvolume;
      previousbedvolume = bedVolume;

     updateDisplay(String(hardness), String(ec), String(bedVolume, 4), hours, minutes, seconds);

     previousMillis = currentTime;
      /*
     if(TDucerVoltage >= 30){
       genie.WriteObject(GENIE_OBJ_LED, 9, 1);
     }
     else{
       genie.WriteObject(GENIE_OBJ_LED, 9, 0);
     }*
    }

      if(AUTOMODETRANS){
        //do something
      }

      if(AUTOMODETRANS){
        //do something
      }*/
    }
  }
}

void updateDisplay(String hardnessStr, String ecStr, String bedVolumeStr, unsigned long hours, unsigned long minutes, unsigned long seconds) 
{
  genie.WriteStr(2, String(hours) + ":" + (minutes < 10 ? "0" : "") + String(minutes) + ":" + (seconds < 10 ? "0" : "" ) + String(seconds));
  genie.WriteStr(3, hardnessStr);
  genie.WriteStr(4, ecStr);
  genie.WriteStr(5, bedVolumeStr);
  genie.WriteStr(6, "SomeValue"); // 
}


void IXControls()
{
genieFrame Event;
  if (genie.DequeueEvent(&Event)) {
    if (Event.reportObject.cmd == GENIE_REPORT_EVENT) {
      if(Event.reportObject.object == GENIE_OBJ_4DBUTTON){
        if (Event.reportObject.index == 0){
          ButtonState0 = genie.GetEventData(&Event);
          if (ButtonState0 == 0){
            LatchSHFT = HIGH;
            LatchRNS = HIGH;
            LatchREG = HIGH;
            LatchSRV = HIGH;
            LatchPURGE = HIGH;
            LatchAUTO = HIGH;
            LatchMastPower = HIGH;
            genie.WriteObject(GENIE_OBJ_ROTARYSW, 0x00, 0);
            genie.WriteObject(GENIE_OBJ_LED, 0, 0);
            genie.WriteObject(GENIE_OBJ_LED, 1, 0);
            genie.WriteObject(GENIE_OBJ_LED, 2, 0);
            genie.WriteObject(GENIE_OBJ_LED, 3, 0);
            genie.WriteObject(GENIE_OBJ_LED, 4, 0);
            genie.WriteObject(GENIE_OBJ_LED, 5, 0);
            buttonStartStop = false;

          }
          else if (ButtonState0 == 1){
            LatchMastPower = LOW;
            genie.WriteObject(GENIE_OBJ_ROTARYSW, 0x00, 0);
            genie.WriteObject(GENIE_OBJ_LED, 0, 1);
          }
        }

        if(Event.reportObject.index == 1){
          int ButtonState1 = genie.GetEventData(&Event);
          if (ButtonState1 == 0){
            LatchSHFT = HIGH; //slider is set to steady state
          }
          else if (ButtonState1 == 1){
            LatchSHFT = LOW; //slider is set to tansient
          }
        }
        if(Event.reportObject.index == 2){
            buttonStartStop = false;
            genie.WriteObject(GENIE_OBJ_LED, 0, 1);
            genie.WriteObject(GENIE_OBJ_LED, 1, 0);
            genie.WriteObject(GENIE_OBJ_LED, 2, 0);
            genie.WriteObject(GENIE_OBJ_LED, 3, 0);
            genie.WriteObject(GENIE_OBJ_LED, 4, 0);
            genie.WriteObject(GENIE_OBJ_LED, 5, 0);
        }
        if(Event.reportObject.index == 3){

            //Steady State-------------------------------------------------------------------------------------------------------------

            //Rinse Mode
            if(LatchSHFT == HIGH && LatchRNS == LOW && LatchMastPower == LOW && buttonStartStop == false){
              HCPCA9685.Servo(VALVE1, BPOS);  //Reset to APOS
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE2, BPOS);
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE3, APOS);
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE4, APOS);
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE5, APOS);
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE6, APOS);
              delay(ValveStagger);
              //starts counting from when the button is pressed to begin displaying values
              startTime = millis();
              buttonStartStop = true;
              genie.WriteObject(GENIE_OBJ_LED, 0, 0);
              genie.WriteObject(GENIE_OBJ_LED, 1, 1);
              genie.WriteObject(GENIE_OBJ_LED, 2, 0);
              genie.WriteObject(GENIE_OBJ_LED, 3, 0);
              genie.WriteObject(GENIE_OBJ_LED, 4, 0);
              genie.WriteObject(GENIE_OBJ_LED, 5, 0);
            }
            
            //Regen mode
            if(LatchSHFT == HIGH && LatchREG == LOW && LatchMastPower == LOW && buttonStartStop == false){
              HCPCA9685.Servo(VALVE1, APOS);
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE2, BPOS);
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE3, APOS);
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE4, APOS);
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE5, BPOS);
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE6, APOS);
              delay(ValveStagger);
              startTime = millis();
              buttonStartStop = true;
              genie.WriteObject(GENIE_OBJ_LED, 0, 0);
              genie.WriteObject(GENIE_OBJ_LED, 1, 0);
              genie.WriteObject(GENIE_OBJ_LED, 2, 1);
              genie.WriteObject(GENIE_OBJ_LED, 3, 0);
              genie.WriteObject(GENIE_OBJ_LED, 4, 0);
              genie.WriteObject(GENIE_OBJ_LED, 5, 0);
            }

            //Service Mode
            if(LatchSHFT == HIGH && LatchSRV == LOW && LatchMastPower == LOW && buttonStartStop == false){
              HCPCA9685.Servo(VALVE1, BPOS);
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE2, BPOS);
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE3, APOS);
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE4, BPOS);
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE5, BPOS);
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE6, APOS);
              delay(ValveStagger);
              startTime = millis();
              buttonStartStop = true;
              genie.WriteObject(GENIE_OBJ_LED, 0, 0);
              genie.WriteObject(GENIE_OBJ_LED, 1, 0);
              genie.WriteObject(GENIE_OBJ_LED, 2, 0);
              genie.WriteObject(GENIE_OBJ_LED, 3, 1);
              genie.WriteObject(GENIE_OBJ_LED, 4, 0);
              genie.WriteObject(GENIE_OBJ_LED, 5, 0);
            }

            //Purge Mode
            if(LatchSHFT == HIGH && LatchPURGE == LOW && LatchMastPower == LOW && buttonStartStop == false){
              HCPCA9685.Servo(VALVE1, BPOS);
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE2, BPOS);
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE3, APOS);
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE4, BPOS);
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE5, APOS);
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE6, BPOS);
              delay(ValveStagger);
              startTime = millis();
              buttonStartStop = true;
              genie.WriteObject(GENIE_OBJ_LED, 0, 0);
              genie.WriteObject(GENIE_OBJ_LED, 1, 0);
              genie.WriteObject(GENIE_OBJ_LED, 2, 0);
              genie.WriteObject(GENIE_OBJ_LED, 3, 0);
              genie.WriteObject(GENIE_OBJ_LED, 4, 1);
              genie.WriteObject(GENIE_OBJ_LED, 5, 0);
            }

            //Auto Mode
            if(LatchSHFT == HIGH && LatchAUTO == LOW && LatchMastPower == LOW && buttonStartStop == false){
              //do somethings to determine when to switch between modes.
              startTime = millis();
              buttonStartStop = true;
              genie.WriteObject(GENIE_OBJ_LED, 5, 1);
              AUTOMODESTEADY = true;
            }
            else{
              AUTOMODESTEADY = false;
            }


            //Transient-------------------------------------------------------------------------------------------------------------

            //Rinse Mode
            if(LatchSHFT == LOW && LatchRNS == LOW && LatchMastPower == LOW && buttonStartStop == false){
              
              HCPCA9685.Servo(VALVE1, BPOS);  //Reset to APOS
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE2, BPOS);
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE3, BPOS);  //go to drain
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE4, APOS);
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE5, APOS);  
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE6, APOS);
              delay(ValveStagger);
              startTime = millis();
              buttonStartStop = true;
              genie.WriteObject(GENIE_OBJ_LED, 0, 0);
              genie.WriteObject(GENIE_OBJ_LED, 1, 1);
              genie.WriteObject(GENIE_OBJ_LED, 2, 0);
              genie.WriteObject(GENIE_OBJ_LED, 3, 0);
              genie.WriteObject(GENIE_OBJ_LED, 4, 0);
              genie.WriteObject(GENIE_OBJ_LED, 5, 0);
            }

            //Regen Mode
            if(LatchSHFT == LOW && LatchREG == LOW && LatchMastPower == LOW && buttonStartStop == false){
              
              HCPCA9685.Servo(VALVE1, APOS);
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE2, BPOS);
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE3, BPOS);  //go to drain
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE4, APOS);
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE5, BPOS);
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE6, APOS);
              delay(ValveStagger);
              startTime = millis();
              buttonStartStop = true;
              genie.WriteObject(GENIE_OBJ_LED, 0, 0);
              genie.WriteObject(GENIE_OBJ_LED, 1, 0);
              genie.WriteObject(GENIE_OBJ_LED, 2, 1);
              genie.WriteObject(GENIE_OBJ_LED, 3, 0);
              genie.WriteObject(GENIE_OBJ_LED, 4, 0);
              genie.WriteObject(GENIE_OBJ_LED, 5, 0);
            }

            //Service Mode
            if(LatchSHFT == LOW && LatchSRV == LOW && LatchMastPower == LOW && buttonStartStop == false){
              
              HCPCA9685.Servo(VALVE1, BPOS);
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE2, BPOS);
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE3, BPOS);  //go to drain
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE4, BPOS);
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE5, BPOS);
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE6, APOS);
              delay(ValveStagger);
              startTime = millis();
              buttonStartStop = true;
              genie.WriteObject(GENIE_OBJ_LED, 0, 0);
              genie.WriteObject(GENIE_OBJ_LED, 1, 0);
              genie.WriteObject(GENIE_OBJ_LED, 2, 0);
              genie.WriteObject(GENIE_OBJ_LED, 3, 1);
              genie.WriteObject(GENIE_OBJ_LED, 4, 0);
              genie.WriteObject(GENIE_OBJ_LED, 5, 0);
            }

            //Purge Mode
            if(LatchSHFT == LOW && LatchPURGE == LOW && LatchMastPower == LOW && buttonStartStop == false){

              HCPCA9685.Servo(VALVE1, APOS);
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE2, APOS);
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE3, BPOS);
              delay(ValveStagger);
              HCPCA9685.Servo(VALVE4, APOS);
              delay(ValveStagger);
              startTime = millis();
              buttonStartStop = true;
              genie.WriteObject(GENIE_OBJ_LED, 0, 0);
              genie.WriteObject(GENIE_OBJ_LED, 1, 0);
              genie.WriteObject(GENIE_OBJ_LED, 2, 0);
              genie.WriteObject(GENIE_OBJ_LED, 3, 0);
              genie.WriteObject(GENIE_OBJ_LED, 4, 1);
              genie.WriteObject(GENIE_OBJ_LED, 5, 0);
            }

            //Auto Mode
            if(LatchSHFT == LOW && LatchAUTO == LOW && LatchMastPower == LOW && buttonStartStop == false){
              startTime = millis();
              buttonStartStop = true;
              genie.WriteObject(GENIE_OBJ_LED, 5, 1);
              AUTOMODETRANS = true;
            }
            else{
              AUTOMODETRANS = false;
            }
        }
      }
      if(Event.reportObject.object == GENIE_OBJ_ROTARYSW){
       if(Event.reportObject.index == 0x00){
          if(Event.reportObject.data_lsb == 0)
            {
              if(LatchMastPower == LOW){
              genie.WriteObject(GENIE_OBJ_LED, 0,1);
              LatchRNS = HIGH;
              LatchREG = HIGH;
              LatchSRV = HIGH;
              LatchPURGE = HIGH;
              LatchAUTO = HIGH;
              }
            }
            if(Event.reportObject.data_lsb == 1) //Rinse Selector
            {
              if(LatchMastPower == LOW){
              LatchRNS = LOW;
              LatchREG = HIGH;
              LatchSRV = HIGH;
              LatchPURGE = HIGH;
              LatchAUTO = HIGH;
              }
            }
            if(Event.reportObject.data_lsb == 2) //Regen Selector
            {
              if(LatchMastPower == LOW){
              LatchREG = LOW;
              LatchRNS = HIGH;
              LatchSRV = HIGH;
              LatchPURGE = HIGH;
              LatchAUTO = HIGH;
              }
            }
            
            if(Event.reportObject.data_lsb == 3) //Service Selector
            {
              if(LatchMastPower == LOW){
              LatchSRV = LOW;
              LatchRNS = HIGH;
              LatchREG = HIGH;
              LatchPURGE = HIGH;
              LatchAUTO = HIGH;
              }
            }
            if(Event.reportObject.data_lsb == 4) //Purge Selector
            {
              if(LatchMastPower == LOW){
              LatchPURGE = LOW;
              LatchRNS = HIGH;
              LatchREG = HIGH;
              LatchSRV = HIGH;
              LatchAUTO = HIGH;
              }
            }
            if(Event.reportObject.data_lsb == 5) //Auto Selector
            {
              if(LatchMastPower == LOW){
              LatchAUTO = LOW;
              LatchRNS = HIGH;
              LatchREG = HIGH;
              LatchSRV = HIGH;
              LatchPURGE = HIGH;
              }
            }
            
        }
      }
    }
  }
}
