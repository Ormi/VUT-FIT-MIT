/*****************************/
/*   @author Michal Ormos    */
/*  @SEN Project RGBW sensor */
/*      @date Dec 2017       */
/*       @VUT FIT MIT        */ 
/*****************************/

#include "Wire.h"
#include "veml6040.h"

VEML6040 RGBWSensor;
int sensorPin = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin(); 
  if(!RGBWSensor.begin()) {
    Serial.println("ERROR: couldn't detect the sensor");
    while(1){}
  }
   
  /**
   * 320m integration time, auto mode, color sensor 
   * R/G/B colors  | CCT Correlated color temp | AL Ambinet light in lux
   */
  RGBWSensor.setConfiguration(VEML6040_IT_320MS + VEML6040_AF_AUTO + VEML6040_SD_ENABLE);
  delay(1500);
}

void loop() {
  /* Uncoment for RGBW sensor debug mode */
  // Serial.print("RED: ");
  // Serial.print(RGBWSensor.getRed());  
  // Serial.print(" GREEN: ");
  // Serial.print(RGBWSensor.getGreen());  
  // Serial.print(" BLUE: ");
  // Serial.print(RGBWSensor.getBlue());  
  // Serial.print(" WHITE: ");
  // Serial.print(RGBWSensor.getWhite()); 
  // Serial.print(" CCT: ");
  // Serial.print(RGBWSensor.getCCT());  
  // Serial.print(" AL: ");
  // Serial.println(RGBWSensor.getAmbientLight()); 
  // delay(400);

 /**
  * Getting the voltage reading from the temperature sensor
  * Converting that reading to voltage, for 3.3v arduino use 3.3
  * Converting from 10 mv per degree wit 500 mV offset to degrees ((voltage - 500mV) times 100)
  */
 int reading = analogRead(sensorPin);  
 float voltage = reading * 5.0;
 voltage /= 1024.0; 
 float temperatureC = (voltage - 0.5) * 100 ;


 /**
  * Main system RGBW logic circuit
  * Sunny vs. Cloudy - Blue color raise
  */
 if (RGBWSensor.getRed() > 2500 && RGBWSensor.getGreen() > 2500 && RGBWSensor.getBlue() > 45000) {
    Serial.print("1");
 } else {
    Serial.print("0");
 }

 Serial.print(temperatureC);
 Serial.print(",");
 delay(1000);
}









