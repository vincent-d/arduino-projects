
/*
 * SevenSeg.cpp
 */

#include "SevenSeg.h"
#include <SPI.h>

SevenSeg Disp;

void extPrintValueSync() {
	Disp.printValueSync(); 
}

SevenSeg::SevenSeg() {

	SPI.begin();
	SPI.setBitOrder(MSBFIRST);
	SPI.setDataMode(SPI_MODE0);

	m_dot = 0;
	m_isChar = 0;
	m_isNeg = 0;

}

void SevenSeg::printValueSync() {

	int value;
	unsigned int val;
	int i = 0;

	if (m_isChar) {
		// Char / string
		val = m_value.tab[0];
		while (val != 0 && i < DISP_SIZE) {
			spiSendAtIndex(val & 0xFF, i);
			val = m_value.tab[++i];
		}
	} else {
		// Numeric value
		value = m_value.binVal;
		if (m_isNeg)
			value = -value;
		while (value > 0 && i < DISP_SIZE) {

			val = tabCodes[value % 10];
			if (m_dot & 1 << i)
				val &= ~0x80;

			spiSendAtIndex(val, i);

			value /= 10;
			i++;
		}
		if (m_isNeg)
			spiSendAtIndex(tabCodes[NB_CHAR-1], i); // Carret is the last one
	}

	return;

}

void SevenSeg::spiSendAtIndex(unsigned int val, int index){

	digitalWrite(SS, LOW);

	SPI.transfer((0x1 << (DISP_SIZE - 1 - index)) & 0xFF);
	SPI.transfer(val & 0xFF);

	digitalWrite(SS, HIGH);

}

int SevenSeg::setValue(long value) {

	m_isChar = 0;

	if (value < 0)
		m_isNeg = 1;
	else
		m_isNeg = 0;

	m_value.binVal = value;
	return value;

}

int SevenSeg::setValue(char value) {

	int v;

	m_isChar = 1;

	v = charToVal(value);
	m_value.binVal = v >= 0 ? v : 0;
	return m_value.binVal;

}

int SevenSeg::setValue(char str[]) {

	int i = 0, j = 0;
	int v = 0;
	char c;
	char dot = 0xFF; // needed to mask the dot value

	m_isChar = 1;
	m_value.binVal = 0;

	if (str == NULL)
		return -1;

	c = str[0];
	// Look for the end of the string (max 2 DISP_SIZE if there is some dots)
	while (c != '\0' && i < 2 * DISP_SIZE) {
		c = str[++i];
	}
	// Write char from last to first because last on will be sent first
	j = 0;
	for (i--; i >= 0 && j < DISP_SIZE; i--) {
		c = str[i];
		if (c != '.') {
			v = charToVal(c);
			v = v >= 0 ? v : 0;
			m_value.tab[j] = v & dot;
			dot = 0xFF;
			j++;
		} else {
			dot = 0x7F; // 0x7F is the dot value in tabCodes
		}
	}

	return m_value.binVal;

}

int SevenSeg::setValue(float value, int nbDec) {

	m_isChar = 0;

	if (value < 0.0) {
		m_isNeg = 1;
		value = -value;
	}

	int i;
	for (i = 0; i < nbDec; i++) {
		value *= 10;
	}
	m_value.binVal = (unsigned long) value;
	m_dot = nbDec == 0 ? 0 : 1 << (nbDec);

	return m_value.binVal;

}

int SevenSeg::charToVal(char c) {

	int i;
	for (i = 0; i < NB_CHAR; i++) {
		if (tabChar[i] == c)
			return tabCodes[i];
	}
	return -1;

}

byte SevenSeg::setDots(byte dots) {

	m_dot = dots;
	return m_dot;

}

