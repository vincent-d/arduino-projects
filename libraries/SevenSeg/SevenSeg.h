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

#define DISP_SIZE 8
#define NB_CHAR 28
//  7-seg display
//   0---
// 5 |   | 1
//   6---
// 4 |   | 2
//   3---  7.
// A bit=0 means the led is on (the last one is the dot).
const unsigned int tab_codes[NB_CHAR] = {
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
	0x8E, // F
	0x82, // G (== 6)
	0x89, // H
	0xF9, // I (== 1)
	0xE1, // J
	0xC7, // L
	0xC0, // O (== 0)
	0x8C, // P
	0x92, // S (== 5)
	0xC1, // U
	0x7F, // .
	0xFF, // space
	0xBF, // - (CARRET MUST BE THE LAST ONE IN THIS TAB)
};

const char tab_char[NB_CHAR+1] = "0123456789ABCDEFGHIJLOPSU. -";

union spi_value {
	unsigned char tab[DISP_SIZE];
	unsigned long long binVal;
};

struct seven_seg {
	union spi_value value;
	byte dot;
	int is_char;
	int is_neg;
};


void ss_initialize(struct seven_seg *ss);
void ss_printValueSync(struct seven_seg *ss);
int ss_setValueLong(struct seven_seg *ss, long value);
int ss_setValueChar(struct seven_seg *ss, char value);
int ss_setValueFloat(struct seven_seg *ss, float value, int nb_dec);
int ss_setValueStr(struct seven_seg *ss, char str[]);

byte ss_setDots(struct seven_seg *ss, byte dots);

extern struct seven_seg Disp;

#endif // SEVEN_SEG_H

