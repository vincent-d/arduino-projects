/*
 * Sched.h
 */

#ifndef SCHED_H
#define SCHED_H

//#include "Arduino.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define RESOLUTION 65536    // Timer1 is 16 bit

#define MS_2_US(v) ((v) * 1000)
#define S_2_US(v)  (MS_2_US(v * 1000))

struct SchedFunction {
	void (*fn)();
	struct SchedFunction *nextFn;
	unsigned long periodSlots;
	unsigned long remainingSlots;
};

class Sched {

public:
	void initialize(unsigned long time);
	void registerFunction(void (*f)(), unsigned long period, unsigned long offset = 0);
	void isrCallback();
	struct SchedFunction *createSchedFunction(void (*f)(), unsigned long period, unsigned long offset);
	void deRegisterFunction(void (*f)());

private:
	void resume();
	void setPeriod(long microseconds);

	struct SchedFunction *m_firstFn;
	struct SchedFunction *m_lastFn;
	int m_nbFunctions;

	unsigned long m_period;
	unsigned long m_slot;

};

extern Sched Scheduler;

#endif // SCHED_H
