//Testing Script for AD620 OP-Amp mV-V amplifier
//Used for Water Hardness Sensor 0V set at -150mV 5V set to 150mV
//Typical Hardness Sensor Range -50mV to 0V
//Future Plans include integration into greater controls system for AIX/CIX
//and changing range to -200mV - 200mV
//
//Armando Correa 8/28/2023

const int SignalPin=A0;
const float Factor= 0.00488759; // 0-1023bits = 0-5V ~ 5V/1023bits = 0.00488759 V/bit 
                                // Converts Signal input from bits to V

void setup() {
  Serial.begin(9600);
  pinMode(SignalPin, INPUT);
}

void loop() {
float PinReading=analogRead(SignalPin); //reads in Voltage from AD620 as bits
delay(1000);
PinReading=PinReading*Factor; // bits *(V/bits) = V
Serial.println(PinReading);
delay(1000);


}
