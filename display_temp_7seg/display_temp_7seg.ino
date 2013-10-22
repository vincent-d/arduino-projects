
#include <SPI.h>
#include "TimerOne.h"
#include "SevenSeg.h"

const int pin_adc = A0;


// General variables
static int const interrupt_slot_time = 8000;
static int const interrupt_tot_slot = 1500;
static int interrupt_slot = 0;

// SPI / display
SevenSeg disp;

// ADC / measure
static const float range_adc = 5.0;
static const int resolution_adc = 1024;
static const float step_adc = 0.001; // V/dC

void interrupt();

void setup() {

  Timer1.attachInterrupt(interrupt, interrupt_slot_time);
  //disp.setValue(0l);
  //disp.setDots(1<<1);
  disp.setValue(-200l);

}

void loop() {
 
  delay(1000);

}

int readTemp() {
   
  int value_adc = analogRead(pin_adc);
  int temperature = (int) ((( (float) value_adc * range_adc) / (float) resolution_adc) / step_adc);
  return temperature;
  
}

void interrupt() {
  
  int temp;
 
  // All interrupts
  disp.printValueSync();
  
  switch (interrupt_slot) {
    case 0:
      temp = readTemp();
      //disp.setValue((long) temp);
      break;
    default:
      break;    
  }
  
  interrupt_slot = (interrupt_slot + 1) % interrupt_tot_slot;
  
  return;
  
}



