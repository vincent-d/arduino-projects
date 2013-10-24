
/*
 * SevenSeg.cpp
 */

#include <Arduino.h>
#include "SevenSeg.h"
#include "SPIc.h"

struct seven_seg Disp;

static void spiSendAtIndex(unsigned int val, int index);
static int charToVal(char c);

void ss_initialize(struct seven_seg *ss) {

	spi_begin();
	spi_setBitOrder(MSBFIRST);
	spi_setDataMode(SPI_MODE0);

	ss->dot = 0;
	ss->is_char = 0;
	ss->is_neg = 0;

}

void ss_printValueSync(struct seven_seg *ss) {

	int value;
	unsigned int val;
	int i = 0;

	if (ss->is_char) {
		// Char / string
		val = ss->value.tab[0];
		while (val != 0 && i < DISP_SIZE) {
			spiSendAtIndex(val & 0xFF, i);
			val = ss->value.tab[++i];
		}
	} else {
		// Numeric value
		value = ss->value.binVal;
		if (ss->is_neg)
			value = -value;
		while (value > 0 && i < DISP_SIZE) {

			val = tab_codes[value % 10];
			if (ss->dot & 1 << i)
				val &= ~0x80;

			spiSendAtIndex(val, i);

			value /= 10;
			i++;
		}
		if (ss->is_neg)
			spiSendAtIndex(tab_codes[NB_CHAR-1], i); // Carret is the last one
	}

	return;

}

static void spiSendAtIndex(unsigned int val, int index){

	digitalWrite(SS, LOW);

	spi_transfer((0x1 << (DISP_SIZE - 1 - index)) & 0xFF);
	spi_transfer(val & 0xFF);

	digitalWrite(SS, HIGH);

}

int ss_setValueLong(struct seven_seg *ss, long int value) {

	ss->is_char = 0;

	if (value < 0)
		ss->is_neg = 1;
	else
		ss->is_neg = 0;

	ss->value.binVal = value;
	return value;

}

int ss_setValueChar(struct seven_seg *ss, char value) {

	int v;

	ss->is_char = 1;

	v = charToVal(value);
	ss->value.binVal = v >= 0 ? v : 0;
	return ss->value.binVal;

}

int ss_setValueStr(struct seven_seg *ss, char str[]) {

	int i = 0, j = 0;
	int v = 0;
	char c;
	char dot = 0xFF; // needed to mask the dot value

	ss->is_char = 1;
	ss->value.binVal = 0;

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
			ss->value.tab[j] = v & dot;
			dot = 0xFF;
			j++;
		} else {
			dot = 0x7F; // 0x7F is the dot value in tabCodes
		}
	}

	return ss->value.binVal;

}

int ss_setValueFloat(struct seven_seg *ss, float value, int nb_dec) {

	ss->is_char = 0;

	if (value < 0.0) {
		ss->is_neg = 1;
		value = -value;
	}

	int i;
	for (i = 0; i < nb_dec; i++) {
		value *= 10;
	}
	ss->value.binVal = (unsigned long) value;
	ss->dot = nb_dec == 0 ? 0 : 1 << (nb_dec);

	return ss->value.binVal;

}

static int charToVal(char c) {

	int i;
	for (i = 0; i < NB_CHAR; i++) {
		if (tab_char[i] == c)
			return tab_codes[i];
	}
	return -1;

}

byte ss_setDots(struct seven_seg *ss, byte dots) {

	ss->dot = dots;
	return ss->dot;

}

