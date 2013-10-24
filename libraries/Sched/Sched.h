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

#ifdef __cplusplus
extern "C" {
#endif

struct SchedTask {
    void (*fn)();
    struct SchedTask *next_fn;
    unsigned long period_tick;
    unsigned long remaining_ticks;
};

struct Sched {
    struct SchedTask *first_fn;
    struct SchedTask *last_fn;
    int nb_fn;
    unsigned long period;
};

void initialize(struct Sched *sched, unsigned long time);
void registerFunction(struct Sched *sched, void (*f)(), unsigned long period);
void registerFunctionWithOffset(struct Sched *sched, void (*f)(), unsigned long period, unsigned long offset);
void isrCallback();
struct SchedTask *createSchedFunction(struct Sched *sched, void (*f)(), unsigned long period, unsigned long offset);
void deRegisterFunction(struct Sched *sched, void (*f)());
void setPeriod(long microseconds);

// This is the infinite loop. will never return.
void launchScheduler();

extern struct Sched Scheduler;

#ifdef __cplusplus
}
#endif

#endif // SCHED_H
