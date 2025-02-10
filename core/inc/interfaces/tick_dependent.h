#ifndef TICK_DEPENDENT_H
#define TICK_DEPENDENT_H

#include "object_interfaces.h"

/*
 * TickDependent - Interface for objects that can provide tick info
 * Provides tick dependent capabilities
 */
typedef struct TickDependent {
    TickCounter *tick_counter;
    int (*get_current_tick)(TickCounter *tick_counter);
} TickDependent;

static inline TickDependent *TICK_DEPENDENT(const void *object) {
    return GET_INTERFACES(object)->tick_dependent;
}

static inline bool IS_TICK_DEPENDENT(const void *object) {
    return GET_INTERFACES(object)->capabilities.requires_ticks;
}

static inline void SET_TICK_COUNTER(void *object, TickCounter *tick_counter) {
    TICK_DEPENDENT(object)->tick_counter = tick_counter;
}

static inline int GET_CURRENT_TICK(const void *object) {
    if (!IS_TICK_DEPENDENT(object)) return -1;
    return TICK_DEPENDENT(object)->get_current_tick(TICK_DEPENDENT(object)->tick_counter);
}

#endif
