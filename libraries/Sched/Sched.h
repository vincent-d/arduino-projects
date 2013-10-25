/*
 * Sched.h
 */

#ifndef SCHED_H
#define SCHED_H

#include <stdint.h>

#define RESOLUTION 65536    // Timer1 is 16 bit

#define MS_2_US(v) ((v) * 1000)
#define S_2_US(v)  (MS_2_US((v) * 1000))

#ifdef __cplusplus
extern "C" {
#endif

struct SchedTask;

/**
 * This structure holds the data for the scheduler.
 *
 * Both pointers @p first_fn and @p last_fn handle the function list
 * The function list is a simple-chained list of struct SchedTask.
 * Functions are sorted by priority.
 * @p nb_fn holds the size of the function list and @p period correspond to the time period
 */
struct Sched {
	struct SchedTask *first_fn;
	struct SchedTask *last_fn;
	uint16_t nb_fn;
	uint32_t period;
};

/**
 * This structure holds the data for a task
 */
struct SchedTask {
	void (*fn)();
	struct SchedTask *next_fn;
	uint32_t period_tick;
	uint32_t remaining_ticks;
	uint16_t priority;
};

/**
 * Initialize the scheduler with the period given in parameter.
 *
 * It sets properly the Timer1 on the arduino Uno
 *
 * @param sched It needs to exists. This function doen't allocate memory.
 * @param time The tick period
 */
void initialize(struct Sched *sched, uint32_t time);

/**
 * Register a function in the scheduler list
 *
 * If some tasks have same @p priority, the first which was registered will have
 * higher priority than others. Highest @p priority is 0.
 *
 * The @p period is given in tick multiple and not in time unit
 */
void registerFunction(struct Sched *sched, void (*f)(), uint32_t period, uint16_t priority);

/**
 * Register a function in the scheduler list an delay the first call by given @p offset
 *
 * The @p period is given in tick multiple and not in time unit
 */
void registerFunctionWithOffset(struct Sched *sched, void (*f)(), uint32_t period, uint16_t priority, uint32_t offset);

/**
 * Create a SchedTask strucuture with given parameters
 */
struct SchedTask *createSchedFunction(void (*f)(), uint32_t period_tick, uint16_t priority, uint32_t offset);

/**
 * De-register a function from the scheduler list
 */
void deRegisterFunction(struct Sched *sched, void (*f)());

/**
 * This is the main scheduler function. It has to be called after initializations
 * This is an infinite loop, it will never return.
 */
void launchScheduler(struct Sched *sched);

#ifdef __cplusplus
}
#endif

#endif // SCHED_H
