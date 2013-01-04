

#include "TimerOne.h"
#include <SPI.h>

const int pin_adc = A0;


//  7-seg display
//   0---
// 5 |   | 1
//   6---
// 4 |   | 2
//   3---  7.
// A bit=0 means the led is on (the last one is the dot).
const unsigned int seven_seg[16] = {
  0xC0, // 0
  0xF9, // 1
  0xA4, // 2
  0xB0, // 3
  0x99, // 4
  0x92, // 5
  0x82, // 6
  0xF8, // 7
  0x80, // 8
  0x90, // 9
  0x88, // A
  0x83, // b
  0xC6, // C
  0xA1, // d
  0x86, // E
  0x8E // F
};

// SPI / display
int value_to_print = 0;
int dot_dec = 1;

// ADC / measure
int temperature=0;
int value_adc;
float range_adc = 5.0;
int resolution_adc = 1024;
float step_adc = 0.001; // V/dC

void interrupt() ;

void setup() {

 SPI.begin();
 SPI.setBitOrder(MSBFIRST);
 SPI.setDataMode(SPI_MODE0);
 
 Timer1.attachInterrupt(interrupt, 8000);

}

void loop() {

  value_adc = analogRead(pin_adc);

  temperature = (int) ((( (float) value_adc * range_adc) / (float) resolution_adc) / step_adc);
  value_to_print = temperature;
  
  delay(1000);

}


void interrupt() {
 
  int value = value_to_print;
  int i = 0;
  unsigned int val;
  
  while (value > 0 && i < 8) {
    
    digitalWrite(SS, LOW);

    val = seven_seg[value % 10];
    if (i != 0 && dot_dec == i)
      val &= ~0x80;
    
    SPI.transfer((0x1 << (7 - i)) & 0xFF);
    SPI.transfer(val & 0xFF);
    
    digitalWrite(SS, HIGH);
    
    value /= 10;
    i++;
  }
  
  return;
  
}
