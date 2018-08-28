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
  printFloat(temp_f);

    delay(9);
}

void printFloat(float x) { // It's like Ledcontrol's printnumber, except for temperature range floats
    int ones;
    int tens;
    int hundreds;
    int tenths;
    boolean negative;  
    negative=false;

    if(x < -999 || x > 9999) 
       return;
    if(x<0) {
        negative=true;
        x=x*-1;
    }
 /*
    int v=int(x*10.0);
    tenths=v%10;
    ones=v%100;
    tens=v%1000;
    hundreds=v%10000;
      */  
    int v=int(x*10.0);
    tenths=v%10;
    v=v/10;
    ones=v%10;
    v=v/10;
    tens=v%10;
    v=v/10;
    hundreds=v%10;
         
    if(negative) {
       //print character '-' in the leftmost column 
       lc.setChar(0,3,'-',false);
       if (hundreds >= 1) {
        lc.setDigit(0,2,(byte)hundreds,false);
        lc.setDigit(0,1,(byte)tens,false);
        lc.setDigit(0,0,(byte)ones,false);
       }
       else {
        if (tens >=1 ) 
          lc.setDigit(0,2,(byte)tens,false);
        else
          lc.setChar(0,2,' ',false);
        lc.setDigit(0,1,(byte)ones,true);
        lc.setDigit(0,0,(byte)tenths,false);
       }
    }
    else {
       //print a blank in the sign column
       if (hundreds >= 1) lc.setDigit(0,3,(byte)hundreds,false);
       else lc.setChar(0,3,' ',false);
      
      if (tens >= 1) lc.setDigit(0,2,(byte)tens,false);
      else lc.setChar(0,2,' ',false);
    lc.setDigit(0,1,(byte)ones,true);
    lc.setDigit(0,0,(byte)tenths,false);
    }
    //Now print the number digit by digit

}
