/*
 * Sched.cpp
 */

#include "Sched.h"
#include <stdlib.h>


Sched Scheduler;

ISR(TIMER1_OVF_vect)          // interrupt service routine that wraps a user defined function supplied by attachInterrupt
{
	Scheduler.isrCallback();
}

void Sched::initialize(unsigned long time){

	m_period = time;
	m_nbFunctions = 0;
	m_firstFn = NULL;
	m_lastFn = NULL;

	setPeriod(time);
}

void Sched::registerFunction(void (*f)(), unsigned long period, unsigned long offset){

	if (m_firstFn != NULL) {
		m_lastFn->nextFn = createSchedFunction(f, period, offset);
		m_lastFn = m_lastFn->nextFn;
	} else {
		m_firstFn = createSchedFunction(f, period, offset);
		m_lastFn = m_firstFn;
	}
	m_nbFunctions++;

	return;
}

struct SchedFunction *Sched::createSchedFunction(void (*f)(), unsigned long period, unsigned long offset) {

	struct SchedFunction *s;
	int p;
	s = (struct SchedFunction*) malloc(sizeof(struct SchedFunction));
	s->fn = f;
	s->nextFn = NULL;
	p = period / m_period;
	s->periodSlots = p > 0 ? p : 1;
	s->remainingSlots = offset;

	return s;
}

void Sched::deRegisterFunction(void (*f)()) {

	struct SchedFunction *s = m_firstFn;
	struct SchedFunction *p = m_firstFn;
	int i;

	if (m_firstFn->fn == f) {
		m_firstFn = m_firstFn->nextFn;
		free(s);
	} else {
		s = m_firstFn->nextFn;
		for (i = 0; i < m_nbFunctions - 1; i++) {
			if (s->fn == f) {
				p->nextFn = s->nextFn;
				if (s == m_lastFn)
					m_lastFn = p;
				free(s);
				break;
			}
			p = s;
			s = s->nextFn;
		}
	}
	m_nbFunctions--;

}

void Sched::isrCallback() {

	int i;
	struct SchedFunction *f = m_firstFn;
	for (i = 0; i < m_nbFunctions; i++) {
		if (f->remainingSlots == 0) {
			f->fn();
			f->remainingSlots = f->periodSlots - 1;
		} else {
			f->remainingSlots--;
		}
		f = f->nextFn;
	}
}

void Sched::setPeriod(long microseconds)		// AR modified for atomic access
{
	unsigned char clockSelectBits;
	char oldSREG;					// To hold Status Register while ints disabled
	long cycles = (F_CPU / 2000000) * microseconds;                                // the counter runs backwards after TOP, interrupt is at BOTTOM so divide microseconds by 2
	if(cycles < RESOLUTION)              clockSelectBits = _BV(CS10);              // no prescale, full xtal
	else if((cycles >>= 3) < RESOLUTION) clockSelectBits = _BV(CS11);              // prescale by /8
	else if((cycles >>= 3) < RESOLUTION) clockSelectBits = _BV(CS11) | _BV(CS10);  // prescale by /64
	else if((cycles >>= 2) < RESOLUTION) clockSelectBits = _BV(CS12);              // prescale by /256
	else if((cycles >>= 2) < RESOLUTION) clockSelectBits = _BV(CS12) | _BV(CS10);  // prescale by /1024
	else        cycles = RESOLUTION - 1, clockSelectBits = _BV(CS12) | _BV(CS10);  // request was out of bounds, set as maximum

	oldSREG = SREG;
	cli();							// Disable interrupts for 16 bit register access
	ICR1 = cycles;                                          // ICR1 is TOP in p & f correct pwm mode
	SREG = oldSREG;

	TCCR1B &= ~(_BV(CS10) | _BV(CS11) | _BV(CS12));

	TIMSK1 = _BV(TOIE1);                                     // sets the timer overflow interrupt enable bit

	TCCR1B |= clockSelectBits;                                          // reset clock select register, and starts the clock
}
