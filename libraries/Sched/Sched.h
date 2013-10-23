/*
 * Sched.h
 */

#ifndef SCHED_H
#define SCHED_H

//#include "Arduino.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define RESOLUTION 65536    // Timer1 is 16 bit

struct Slot;
struct SlotFunction;

struct Slot {
	struct SlotFunction *functions;
	struct Slot *nextSlot;
	int nbFunctions;
};

struct Slot *createSlotList(int nb);
struct Slot *createEmptySlot();

struct SlotFunction {
	void (*fn)();
	struct SlotFunction *nextFn;
};

struct SlotFunction *createSlotFunction(void (*f)());

class Sched {

public:
	void initialize(int time, int nbSlots);

	void registerFunctionInSlot(void (*f)(), int slot);
	void registerFunctionInAllSlots(void (*f)());

	void isrCallback();

private:
	void resume();
	void setPeriod(long microseconds);

	unsigned char clockSelectBits;
	char oldSREG;					// To hold Status Register while ints disabled

	int m_nbSlots;
	struct Slot *m_firstSlot;
	struct Slot *m_currentSlot;
	void internalRegisterFnInSlot(void (*f)(), struct Slot *s);

};

extern Sched Scheduler;

#endif // SCHED_H
