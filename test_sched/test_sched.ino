
#include <SPI.h>
#include "SevenSeg.h"
#include "Sched.h"

const int pin_adc = A0;

// ADC / measure
static const float range_adc = 5.0;
static const int resolution_adc = 1024;
static const float step_adc = 0.01; // V/°C

void updateTemp();

void setup() {

  Disp.setValue(0l);
  Scheduler.initialize(MS_2_US(8));
  Scheduler.registerFunction(updateTemp, S_2_US(2), 1875);
  Scheduler.registerFunction(extPrintValueSync, MS_2_US(8));

}

void loop() {
 
  delay(1000);

}

float readTemp() {
   
  int value_adc = analogRead(pin_adc);
  float temperature = ((( (float) value_adc * range_adc) / (float) resolution_adc) / step_adc);
  return temperature;
  
}

void updateTemp() {
  float t = readTemp();
  Disp.setValue(t);
}

