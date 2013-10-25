/*
 * Sched.c
 */

#include "Sched.h"
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>


static volatile uint8_t ticks = 0;

ISR(TIMER1_OVF_vect)          // interrupt service routine that wraps a user defined function supplied by attachInterrupt
{
	isrCallback();
}

void initialize(struct Sched* sched, uint32_t time){

	if (sched == NULL)
		return;

	sched->period = time;
	sched->nb_fn = 0;
	sched->first_fn = NULL;
	sched->last_fn = NULL;

	setPeriod(time);
}

void registerFunction(struct Sched *sched, void (*f)(), uint32_t period, uint16_t priority) {
	registerFunctionWithOffset(sched, f, period, priority, 0);
}

void registerFunctionWithOffset(struct Sched *sched, void (*f)(), uint32_t period, uint16_t priority, uint32_t offset){

	struct SchedTask *task;
	struct SchedTask *prev_task = sched->first_fn;

	task = createSchedFunction(f, period, priority, offset);

	if (sched->first_fn != NULL) {
		if (prev_task->priority > priority) {
			insertTask(sched, task, sched->first_fn);
		} else {
			while (prev_task->priority <= priority && prev_task != sched->last_fn)
				prev_task = prev_task->next_fn;
			appendTask(sched, task, prev_task);
		}
	} else {
		/* First task added */
		sched->first_fn = task;
		sched->last_fn = sched->first_fn;
	}
	sched->nb_fn++;
}

static void insertTask(struct Sched *sched, struct SchedTask *task, struct SchedTask *ref) {

	struct SchedTask *t = sched->first_fn;
	if (t == ref) {
		sched->first_fn = task;
	} else {
		while (t->next_fn != ref)
			t = t->next_fn;
		t->next_fn = task;
	}
	task->next_fn = ref;
}

static void appendTask(struct Sched *sched, struct SchedTask *task, struct SchedTask *ref) {

	if (ref == sched->last_fn){
		sched->last_fn = task;
	}
	task->next_fn = ref->next_fn;
	ref->next_fn = task;
}

struct SchedTask *createSchedFunction(void (*f)(), uint32_t period_tick, uint16_t priority, uint32_t offset) {

	struct SchedTask *s;
	int p;
	s = (struct SchedTask*) malloc(sizeof(struct SchedTask));
	s->fn = f;
	s->next_fn = NULL;
	p = period_tick;
	s->period_tick = p > 0 ? p : 1;
	s->remaining_ticks = offset + 1; //delay all tasks to avoid underflow on the first call
	s->priority = priority;

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

static void isrCallback() {

	ticks++;
}


void launchScheduler(struct Sched *sched) {

	int i;
	struct SchedTask *f;

	while (1) {
		set_sleep_mode(SLEEP_MODE_IDLE);
		sleep_mode();

		cli();
		while (ticks > 0) {
			f = sched->first_fn;
			for (i = 0; i < sched->nb_fn; i++) {
				if (f->remaining_ticks == 0) {
					f->fn();
					f->remaining_ticks = f->period_tick - 1;
				} else {
					f->remaining_ticks--;
				}
				f = f->next_fn;
			}
			ticks--;
		}
		sei();
	}
}

static void setPeriod(uint32_t microseconds)		// AR modified for atomic access
{
	uint8_t clockSelectBits;
	uint8_t oldSREG;					// To hold Status Register while ints disabled
	uint32_t cycles = (F_CPU / 2000000) * microseconds;                                // the counter runs backwards after TOP, interrupt is at BOTTOM so divide microseconds by 2
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
