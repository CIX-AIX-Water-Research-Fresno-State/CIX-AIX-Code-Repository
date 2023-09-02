//Armando Correa
// 
// Purpose: Try to stop the Service run based on increased derivative. Threshold is a number tbd
//

const int WH=A0;
const int switch_led=2;
const int elapsed_time=1;
int first_time=0;
int counter=0;
const int threshold =10;
long hardness_total=0;
long hardness=0;
int samplenum=0;
int hardness_old=0;
int delta_hardness=0;
int derivative_counter=0;
int deriv_threshold=2; //should be three or 4

void setup() {
Serial.begin(9600);

pinMode(switch_led,OUTPUT);
int hardness_total=0;
int hardness=0;

}

void loop() {

for (int ii=1; ii<=5000; ii=ii+1) {
 hardness=analogRead(WH);
 hardness_total=hardness_total+hardness;
 delay(3);
 samplenum=samplenum+1;
// Serial.println(samplenum);
 //Serial.println(hardness_total);
}

//Serial.println("Hardness Total");
//Serial.println(hardness_total);
int hardness_avg=hardness_total/samplenum;
int hardness_new=hardness_avg;

Serial.println("Hardness_avg");
Serial.println(hardness_avg);
Serial.println("outsideloop");

if (hardness_new > 0) {
  delta_hardness=hardness_new-hardness_old;
  hardness_old=hardness_new;
}

Serial.println("delta");
Serial.println(delta_hardness);

hardness_total=0;
samplenum=0;

if (delta_hardness >= threshold) {
  derivative_counter=derivative_counter+1;
  Serial.println("delta is big");
}
Serial.println("deriv counter it");
Serial.println(derivative_counter);

if(derivative_counter > deriv_threshold) {
  Serial.println("stop service cycle");
  derivative_counter=0;
}



}
