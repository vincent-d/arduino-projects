/*
 * Sched.cpp
 */

#include "Sched.h"
#include <stdlib.h>


struct Slot *createSlotList(int nb) {

	struct Slot *first;
	struct Slot *s;
	int i;

	first = createEmptySlot();

	s = first;
	for (i = 1; i < nb; i++) {
		s->nextSlot = createEmptySlot();
	}
	s->nextSlot = first;

	return first;
}

struct Slot *createEmptySlot() {

	struct Slot *s = (struct Slot*) malloc(sizeof(struct Slot));
	s->nbFunctions = 0;
	s->functions = NULL;

	return s;
}

Sched Scheduler;

ISR(TIMER1_OVF_vect)          // interrupt service routine that wraps a user defined function supplied by attachInterrupt
{
	Scheduler.isrCallback();
}

void Sched::initialize(int time, int nbSlots){


	m_nbSlots = nbSlots;
	m_firstSlot = createSlotList(nbSlots);
	m_currentSlot = m_firstSlot;

	setPeriod(time);
	TIMSK1 = _BV(TOIE1);                                     // sets the timer overflow interrupt enable bit
	// AR - remove sei() - might be running with interrupts disabled (eg inside an ISR), so leave unchanged
	//  sei();                                                   // ensures that interrupts are globally enabled
	resume();

}

void Sched::registerFunctionInSlot(void (*f)(), int slot){

	int i;
	struct Slot *s = m_firstSlot;

	for (i = 0; i < slot; i++){
		s = s->nextSlot;
	}

	internalRegisterFnInSlot(f, s);

	return;
}

void Sched::registerFunctionInAllSlots(void (*f)()){

	struct Slot *s = m_firstSlot;
	int i;
	for (i = 0; i < m_nbSlots; i++) {
		internalRegisterFnInSlot(f, s);
		s = s->nextSlot;
	}
}

void Sched::internalRegisterFnInSlot(void (*f)(), struct Slot *s) {

	struct SlotFunction *p;
	int i;

	if (s->nbFunctions) {
		p = s->functions;
		for (i = 0; i < s->nbFunctions - 1; i++) {
			p = p->nextFn;
		}
		p->nextFn = createSlotFunction(f);
	} else {
		s->functions = createSlotFunction(f);
	}
	s->nbFunctions++;

}

struct SlotFunction *createSlotFunction(void (*f)()) {

	struct SlotFunction *p;
	p = (struct SlotFunction*) malloc(sizeof(struct SlotFunction));
	p->fn = f;
	p->nextFn = NULL;

	return p;
}

void Sched::resume()				// AR suggested
{
	TCCR1B |= clockSelectBits;
}

void Sched::setPeriod(long microseconds)		// AR modified for atomic access
{

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
	TCCR1B |= clockSelectBits;                                          // reset clock select register, and starts the clock
}

void Sched::isrCallback() {

	int i;
	struct SlotFunction *p = m_currentSlot->functions;
	for (i = 0; i < m_currentSlot->nbFunctions; i++) {
		p->fn();
		p = p->nextFn;
	}

	m_currentSlot = m_currentSlot->nextSlot;
}


