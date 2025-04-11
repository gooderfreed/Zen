#include "../zen.h"

/*
 * Initialize tick counter
 * Returns a zeroed TickCounter structure.
 */
TickCounter init_tick_counter(void) {
    TickCounter tick_counter = {0};
    return tick_counter;
}

/*
 * Set ticks per second
 * Configures the desired logic update rate.
 */
void set_ticks_per_second(TickCounter *counter, int ticks) {
    counter->ticks_per_second = ticks;
    counter->current_ticks = 0;
    clock_gettime(CLOCK_MONOTONIC, &counter->last_tick_time);
}

/*
 * Get current tick count
 * Returns the number of ticks elapsed since last reset.
 */
int get_ticks(TickCounter *counter) {
    return counter->current_ticks;
}

/*
 * Check if game logic should update
 * Returns true if enough time has passed for a new tick.
 */
bool should_update_ticks(TickCounter *tick_counter, TimeManager *tm) {
    double elapsed = (double)(tm->current_time.tv_sec - tick_counter->last_tick_time.tv_sec) +
                     (double)(tm->current_time.tv_nsec - tick_counter->last_tick_time.tv_nsec) / 1e9;

    double target_tick_time = 1.0 / tick_counter->ticks_per_second;

    if (elapsed < target_tick_time) return false;

    tick_counter->last_tick_time = tm->current_time;
    tick_counter->current_ticks++;
    if (tick_counter->current_ticks >= tick_counter->ticks_per_second) {
        tick_counter->current_ticks = 0;
    }

    return true;
}
