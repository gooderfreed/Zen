#ifndef TICK_COUNTER_H
#define TICK_COUNTER_H

#include "../components.h"

/*
 * Tick counter structure
 * Manages fixed time step updates for update logic.
 */
typedef struct TickCounter {
    int ticks_per_second;           // Number of logic updates per second
    int current_ticks;              // Number of ticks since last update
    struct timespec last_tick_time; // Timestamp of last tick
} TickCounter;


TickCounter init_tick_counter(void);
void set_ticks_per_second(TickCounter *counter, int ticks);
int get_ticks(TickCounter *counter);
bool should_update_ticks(TickCounter *tick_counter, TimeManager *tm);

#endif
