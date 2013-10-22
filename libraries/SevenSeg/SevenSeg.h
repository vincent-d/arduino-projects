/*
 * SevenSeg.h
 * 
 * Drive the 7segments display through SPI bus
 *
 * Display works with a two-byte transfer.
 * First Byte sent: index of display
 * Second byte sent: value to display at index
 */
 
#ifndef SEVEN_SEG_H
#define SEVEN_SEG_H 

#include "Arduino.h"


#define NB_CHAR 16
//  7-seg display
//   0---
// 5 |   | 1
//   6---
// 4 |   | 2
//   3---  7.
// A bit=0 means the led is on (the last one is the dot).
const unsigned int seven_seg[NB_CHAR] = {
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

const char tab_char[NB_CHAR+1] = "0123456789ABCDEF";

union spiValue {
  unsigned char tab[8];
  unsigned long long binVal;
};

class SevenSeg {
 
  public:
    SevenSeg(int dispSize = 8);
    int printValueSync(int isChar = 0);
    int setValue(long value);
    int setValue(char value);
    int setValue(char str[]);
    
  private:
    int m_size;
    spiValue m_value;
//    unsigned long value_to_print = 0;
    int dot_dec = 1;
    
    int charToVal(char c);
    int spiSendAtIndex(unsigned int val, int index);
  
};

#endif // SEVEN_SEG_H

