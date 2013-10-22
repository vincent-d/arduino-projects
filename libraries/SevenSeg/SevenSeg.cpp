
/*
 * SevenSeg.cpp
 */

#include "SevenSeg.h"
#include <SPI.h>

SevenSeg::SevenSeg(int dispSize) {
  
 SPI.begin();
 SPI.setBitOrder(MSBFIRST);
 SPI.setDataMode(SPI_MODE0);
 m_size = dispSize;
  
}

int SevenSeg::printValueSync(int isChar) {
  
  unsigned int value;
  int i = 0;
  unsigned int val;
  
  if (isChar) {
    value = m_value.tab[0];
    while (value != 0 && i < m_size) {
      
      val = value & 0xFF;
      if (i != 0 && dot_dec == i)
        val &= ~0x80;
    
      spiSendAtIndex(val, i);      
      
      value = m_value.tab[++i];
    }
  } else {
    value = m_value.binVal;
    while (value > 0 && i < m_size) {
      
      val = seven_seg[value % 10];
      if (i != 0 && dot_dec == i)
        val &= ~0x80;
    
      spiSendAtIndex(val, i);      
      
      value /= 10;
      i++;
    }
  }
  
  return 0;
  
}

int SevenSeg::spiSendAtIndex(unsigned int val, int index){
  
  digitalWrite(SS, LOW);
     
  SPI.transfer((0x1 << (m_size - 1 - index)) & 0xFF);
  SPI.transfer(val & 0xFF);
      
  digitalWrite(SS, HIGH);
      
}

int SevenSeg::setValue(long value) { 

  m_value.binVal = value;
  return value;

}

int SevenSeg::setValue(char value) {

  int v = charToVal(value);
  m_value.binVal = v >= 0 ? v : 0;
  return m_value.binVal;

}

int SevenSeg::setValue(char str[]) {
 
  int i = 0, j = 0;
  int v = 0;
  char c = str[0];
  m_value.binVal = 0;
  while (c != '\0' && i < m_size) {
    c = str[++i];
  }
  for (i--,j = 0; i >= 0; i--,j++) {
    c = str[i];
    v = charToVal(c);
    v = v >= 0 ? v : 0;
    m_value.tab[j] = v;
  }
  
  return m_value.binVal;
  
}

int SevenSeg::charToVal(char c) {
 
  int i;
  for (i = 0; i < NB_CHAR; i++) {
    if (tab_char[i] == c) 
      return seven_seg[i];
  }
  return -1;
  
}

