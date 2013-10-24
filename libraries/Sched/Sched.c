/*
 * Sched.c
 */

#include "Sched.h"
#include <stdlib.h>


struct Sched Scheduler;

static volatile int ticks;

ISR(TIMER1_OVF_vect)          // interrupt service routine that wraps a user defined function supplied by attachInterrupt
{
	isrCallback();
}

void initialize(struct Sched *sched, unsigned long time){

	sched->period = time;
	sched->nb_fn = 0;
	sched->first_fn = NULL;
	sched->last_fn = NULL;

	ticks = 0;

	setPeriod(time);
}

void registerFunction(struct Sched *sched, void (*f)(), unsigned long period) {
	registerFunctionWithOffset(sched, f, period, 0);
}

void registerFunctionWithOffset(struct Sched *sched, void (*f)(), unsigned long period, unsigned long offset){

	if (sched->first_fn != NULL) {
		sched->last_fn->next_fn = createSchedFunction(sched, f, period, offset);
		sched->last_fn = sched->last_fn->next_fn;
	} else {
		sched->first_fn = createSchedFunction(sched, f, period, offset);
		sched->last_fn = sched->first_fn;
	}
	sched->nb_fn++;
}

struct SchedTask *createSchedFunction(struct Sched *sched, void (*f)(), unsigned long period, unsigned long offset) {

	struct SchedTask *s;
	int p;
	s = (struct SchedTask*) malloc(sizeof(struct SchedTask));
	s->fn = f;
	s->next_fn = NULL;
	p = period / sched->period;
	s->period_tick = p > 0 ? p : 1;
	s->remaining_ticks = offset;

	return s;
}

void deRegisterFunction(struct Sched *sched, void (*f)()) {

	struct SchedTask *s = sched->first_fn;
	struct SchedTask *p = sched->first_fn;
	int i;

	if (sched->first_fn->fn == f) {
		sched->first_fn = sched->first_fn->next_fn;
		free(s);
	} else {
		s = sched->first_fn->next_fn;
		for (i = 0; i < sched->nb_fn - 1; i++) {
			if (s->fn == f) {
				p->next_fn = s->next_fn;
				if (s == sched->last_fn)
					sched->last_fn = p;
				free(s);
				break;
			}
			p = s;
			s = s->next_fn;
		}
	}
	sched->nb_fn--;
}

void isrCallback() {

	ticks++;
}


void launchScheduler() {

	int i;
	struct SchedTask *f;
	
	while (1) {
		if (ticks) {
			ticks--;
			f = Scheduler.first_fn;
			for (i = 0; i < Scheduler.nb_fn; i++) {
				if (f->remaining_ticks == 0) {
					f->fn();
					f->remaining_ticks = f->period_tick - 1;
				} else {
					f->remaining_ticks--;
				}
				f = f->next_fn;
			}
		}
	}
}

void setPeriod(long microseconds)		// AR modified for atomic access
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
