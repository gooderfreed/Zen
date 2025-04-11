#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include "../components.h"

/*
 * Time manager structure
 * Handles global time tracking for the framework loop.
 */
typedef struct TimeManager {
    struct timespec start_time;    // Time when the game started
    struct timespec last_time;     // Last recorded time step
    struct timespec current_time;  // Current time
} TimeManager;

TimeManager init_time_manager(void);
void update_time_manager(TimeManager *tm);

#endif
