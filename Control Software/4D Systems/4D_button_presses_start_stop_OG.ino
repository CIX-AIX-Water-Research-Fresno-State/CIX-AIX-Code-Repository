#include <genieArduino.h>
#include "HCPCA9685.h"

#define  I2CAdd 0x40

Genie genie;

#define RESETLINE 4
/* I2C slave address for the device/module. For the HCMODU0097 the default I2C address
   is 0x40 */


//Global Variables

unsigned long startTime = 0; // To store the start time
bool counting = false; // Flag to indicate whether counting is active


void setup() {
  Serial.begin(115200); // Initialize serial communication
  genie.Begin(Serial);
  


  pinMode(RESETLINE, OUTPUT);
  digitalWrite(RESETLINE, 1);
  delay(100);
  digitalWrite(RESETLINE, 0);

  delay(5000);
  // Let the display start up after the reset (This is important)
  
  // Set up an event handler for the button press
  genie.AttachEventHandler(buttonStartStopPressHandler);
  //genie.AttachEventHandler(buttonStopPressHandler);
}

void loop() 
{
  // Other code if needed
  genie.DoEvents(); // Process Genie events
  // Your main code logic here

  if (counting) 
  {
    unsigned long currentTime = millis(); // Get the current time
    unsigned long elapsedTime = currentTime - startTime; // Calculate elapsed time
    
    // Calculate hours, minutes, and seconds
    unsigned long hours = elapsedTime / 3600000; // 1 hour = 3600000 milliseconds
    elapsedTime %= 3600000;
    unsigned long minutes = elapsedTime / 60000; // 1 minute = 60000 milliseconds
    elapsedTime %= 60000;
    unsigned long seconds = elapsedTime / 1000; // 1 second = 1000 milliseconds
      
    // Print the elapsed time in hr:min:sec format
    genie.WriteStr(2, String(hours) + ":" + (minutes < 10 ? "0" : "") + String(minutes) + ":" + (seconds < 10 ? "0" : "") + String(seconds));
  }


}


// Event handler for start button press
void buttonStartStopPressHandler() 
{
  genieFrame Event;
  if (genie.DequeueEvent(&Event)) 
  {
    if (Event.reportObject.cmd == GENIE_REPORT_EVENT) 
    {
      if (Event.reportObject.object == GENIE_OBJ_4DBUTTON) 
      {
        if (Event.reportObject.index == 3) 
        {
          startTime = millis();
          counting = true;
          genie.DoEvents();

        }
          if (Event.reportObject.index == 2) 
        {
          counting = false; // Stop counting
          genie.DoEvents();

        }
        
      }
      if (Event.reportObject.object == GENIE_OBJ_ROTARYSW)
      {
          if(Event.reportObject.index == 0x00)
          {
            if(Event.reportObject.data_lsb == 0)
            {
              genie.WriteStr(2, "off");
            }
            if(Event.reportObject.data_lsb == 1)
            {
              genie.WriteStr(3, "rinse");
            }
            if(Event.reportObject.data_lsb == 2)
            {
              genie.WriteStr(4, "reg");
            }
            if(Event.reportObject.data_lsb == 3)
            {
              genie.WriteStr(5, "serv");
            }
            if(Event.reportObject.data_lsb == 4)
            {
              genie.WriteStr(6, "purge");
            }
            if(Event.reportObject.data_lsb == 5)
            {
              genie.WriteStr(0, "auto");
            }
          }

      }
    }
  }
}