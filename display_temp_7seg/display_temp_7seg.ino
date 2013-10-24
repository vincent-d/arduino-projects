
#include "SPIc.h"
#include "TimerOne.h"
#include "SevenSeg.h"

const int pin_adc = A0;


// General variables
static int const interrupt_slot_time = 8000;
static int const interrupt_tot_slot = 1500;
static int interrupt_slot = 0;

// SPI / Display


// ADC / measure
static const float range_adc = 5.0;
static const int resolution_adc = 1024;
static const float step_adc = 0.01; // V/°C

void interrupt();

void setup() {

  Timer1.attachInterrupt(interrupt, interrupt_slot_time);

}

void loop() {

  delay(1000);

}

float readTemp() {

  int value_adc = analogRead(pin_adc);
  float temperature = ((( (float) value_adc * range_adc) / (float) resolution_adc) / step_adc);
  return temperature;

}

void interrupt() {

  float temp;

  // All interrupts
  ss_printValueSync(&Disp);

  switch (interrupt_slot) {
    case 0:
      temp = readTemp();
      ss_setValueFloat(&Disp, temp, 1);
      break;
    default:
      break;
  }

  interrupt_slot = (interrupt_slot + 1) % interrupt_tot_slot;

  return;

}



