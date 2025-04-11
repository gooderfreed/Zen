#include "zen.h"

/*
 * Initialize time manager
 * Sets up the time manager with the current timestamp.
 */
TimeManager init_time_manager(void) {
    TimeManager time_manager = {0};
    clock_gettime(CLOCK_MONOTONIC, &time_manager.start_time);
    time_manager.last_time = time_manager.start_time;
    time_manager.current_time = time_manager.start_time;

    return time_manager;
}

/*
 * Update time manager
 * Records the current time and updates the last recorded time.
 */
void update_time_manager(TimeManager *tm) {
    tm->last_time = tm->current_time;
    clock_gettime(CLOCK_MONOTONIC, &tm->current_time);
}
