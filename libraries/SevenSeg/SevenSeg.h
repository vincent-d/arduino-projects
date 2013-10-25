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

