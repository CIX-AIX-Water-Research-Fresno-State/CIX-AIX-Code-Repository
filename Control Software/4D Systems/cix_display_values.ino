#include <genieArduino.h>

// Assign the pins for the sensors
const int HNpin = A0;
const int ECpin = A1;
const int FloMetpin = A2;

Genie genie;

// Make sure 0 for all
float cumulativeflow = 0.0;
float previousbedvolume = 0.0;
unsigned long startTime;
const unsigned long updateinterval = 15000;
unsigned long previousMillis = 0;

#define RESETLINE 4

void setup() {
  Serial.begin(200000);
  genie.Begin(Serial);
  pinMode(RESETLINE, OUTPUT);
  digitalWrite(RESETLINE, 1);
  delay(100);
  digitalWrite(RESETLINE, 0);

  delay(10000);

  startTime = millis();
}

void loop() {
  // Elapsed Time
  unsigned long currentTime = millis();
  unsigned long elapseTime = currentTime - startTime;
  unsigned long hours = elapseTime / 3600000; // calculating hours 
  elapseTime %= 3600000;
  unsigned long minutes = elapseTime / 60000;// calculate minutes
  elapseTime %= 60000;
  unsigned long seconds = elapseTime / 1000; //calculate seconds

  // Update Display every 15 seconds
  if (currentTime - previousMillis >= updateinterval) {
    float hardness = analogRead(HNpin); // analog in for hardnsss
    float ec = analogRead(ECpin); // analog in for ec
    float FloMetmV = analogRead(FloMetpin); // analog in for flow meter

    float flowRate /* mL/s */ = FloMetmV * 0.03;
    float bedVolume = (flowRate / (4 * 1000) / 530.1438) + previousbedvolume;
    previousbedvolume = bedVolume;

    updateDisplay(String(hardness), String(ec), String(bedVolume, 4), hours, minutes, seconds);

    previousMillis = currentTime;  // This line was removed in the corrected version
  }

  genie.DoEvents();
}


void updateDisplay(String hardnessStr, String ecStr, String bedVolumeStr, unsigned long hours, unsigned long minutes, unsigned long seconds) 
{
  genie.WriteStr(2, String(hours) + ":" + (minutes < 10 ? "0" : "") + String(minutes) + ":" + (seconds < 10 ? "0" : "" ) + String(seconds));
  genie.WriteStr(3, hardnessStr);
  genie.WriteStr(4, ecStr);
  genie.WriteStr(5, bedVolumeStr);
  genie.WriteStr(6, "SomeValue"); // 
}
