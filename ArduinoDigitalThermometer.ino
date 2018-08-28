#include "LedControl.h"

LedControl lc=LedControl(12,11,10,1); 



#define SERIESRESISTOR 9860.0
#define THERMISTORNOMINAL 10000
#define BCOEFFICIENT 3950
#define TEMPERATURENOMINAL 25

#define SENSOR_HISTORY_LENGTH 99
#define MAF 1 // 0 FOR NO MOVING AVERAGE FILTER
int sensor1history[SENSOR_HISTORY_LENGTH];
int sensor1;
int sensorIndex = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(2, OUTPUT);
  
     //wake up the MAX72XX from power-saving mode 
   lc.shutdown(0,false);
   //set a medium brightness for the Leds
   lc.setIntensity(0,8);

   lc.clearDisplay(0);
   
}

void loop() {
  // put your main code here, to run repeatedly:
    digitalWrite(2, HIGH);
  delay(1);

    sensor1history[sensorIndex] = analogRead(1);
      digitalWrite(2, LOW);
      
    long average1 = 0;
    for (int i = 0; i < (SENSOR_HISTORY_LENGTH); i=i+1 ){
      average1 += sensor1history[i];
    }

	if (MAF) sensor1 = average1/SENSOR_HISTORY_LENGTH;
	else sensor1 = sensor1history[sensorIndex];

      if (sensorIndex == SENSOR_HISTORY_LENGTH - 1){
    sensorIndex = 0;
    } 
    else{
      sensorIndex= sensorIndex + 1;
      }

      float res;
  res = SERIESRESISTOR * sensor1 / (1023.0 - sensor1); //ADAFRUIT SUCKS DICKS HERE
  //Serial.print("Thermistor resistance "); 
  //Serial.println(res);

  // this part is okay
  float steinhart;
  steinhart = res / THERMISTORNOMINAL;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert to C


  float temp_f = 9.0/5.0*steinhart+32.0;
  printFloat(temp_f, 0);

    delay(9);
}

void printFloat(float x, int addr) { // It's like Ledcontrol's printnumber, except for temperature range floats
    int ones=0;
    int tens=0;
    int hundreds=0;
    int tenths=0;
    boolean negative;  
    negative=false;

    int offset;
    offset = 0;
    if (addr == 1) offset = 4; 

    if(x < -999 || x > 9999) 
       return;
    if(x<0) {
        negative=true;
        x=x*-1;
    }

    int v=int(x*10.0);
    tenths=v%10;
    ones=v/10%10;
    tens=v/100%10;
    hundreds=v/1000%10;
  
    if(negative) {
       //print character '-' in the leftmost column 
       lc.setChar(0,3+offset,'-',false);
       if (hundreds >= 1) {
        lc.setDigit(0,2+offset,(byte)hundreds,false);
        lc.setDigit(0,1+offset,(byte)tens,false);
        lc.setDigit(0,0+offset,(byte)ones,false);
       }
       else {
        if (tens >=1 ) 
          lc.setDigit(0,2+offset,(byte)tens,false);
        else
          lc.setChar(0,2+offset,' ',false);
        lc.setDigit(0,1+offset,(byte)ones,true);
        lc.setDigit(0,0+offset,(byte)tenths,false);
       }
    }
    else {
       if (hundreds >= 1) lc.setDigit(0,3+offset,(byte)hundreds,false);
       else lc.setChar(0,3+offset,' ',false);
      
      if (tens >= 1 | hundreds >= 1) lc.setDigit(0,2+offset,(byte)tens,false);
      else lc.setChar(0,2+offset,' ',false);
    lc.setDigit(0,1+offset,(byte)ones,true);
    lc.setDigit(0,0+offset,(byte)tenths,false);
    }

}
