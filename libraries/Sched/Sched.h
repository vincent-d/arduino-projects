/*
 * Sched.h
 */

#ifndef SCHED_H
#define SCHED_H

//#include "Arduino.h"
#include <stdint.h>

#define RESOLUTION 65536    // Timer1 is 16 bit

#define MS_2_US(v) ((v) * 1000)
#define S_2_US(v)  (MS_2_US(v * 1000))

#ifdef __cplusplus
extern "C" {
#endif

struct SchedTask {
    void (*fn)();
    struct SchedTask *next_fn;
    uint32_t period_tick;
    uint32_t remaining_ticks;
	uint16_t priority;
};

struct Sched {
    struct SchedTask *first_fn;
    struct SchedTask *last_fn;
    uint16_t nb_fn;
    uint32_t period;
};

void initialize(struct Sched *sched, uint32_t time);
void registerFunction(struct Sched *sched, void (*f)(), uint32_t period, uint16_t priority);
void registerFunctionWithOffset(struct Sched *sched, void (*f)(), uint32_t period, uint16_t priority, uint32_t offset);
void isrCallback();
struct SchedTask *createSchedFunction(void (*f)(), uint32_t period_tick, uint16_t priority, uint32_t offset);
void deRegisterFunction(struct Sched *sched, void (*f)());
void setPeriod(uint32_t microseconds);

// This is the infinite loop. will never return.
void launchScheduler(struct Sched *sched);

static void appendTask(struct Sched *sched, struct SchedTask *task, struct SchedTask *ref);
static void insertTask(struct Sched *sched, struct SchedTask *task, struct SchedTask *ref);

#ifdef __cplusplus
}
#endif

#endif // SCHED_H
