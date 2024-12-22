#ifndef POSITION_HANDLER_H
#define POSITION_HANDLER_H

#include "object_interfaces.h"

/*
 * PositionHandler - Interface for objects that can handle position
 * Provides position restoration and saving capabilities
 */
typedef struct PositionHandler {
    Coords restore_coords;
    void (*restore_pos)(void *self, Coords *current_coords);
    void (*save_current_pos)(void *self, Coords current_coords);
} PositionHandler;

// Position Handler macros
static inline PositionHandler *POSITION_HANDLER(const void *object) {
    return GET_INTERFACES(object)->position_handler;
}

static inline bool IS_POSITIONABLE(const void *object) {
    return GET_INTERFACES(object)->capabilities.is_positionable;
}

static inline void RESTORE_POSITION(void *object, Coords *coords) {
    POSITION_HANDLER(object)->restore_pos(object, coords);
}

static inline void SAVE_POSITION(void *object, Coords coords) {
    POSITION_HANDLER(object)->save_current_pos(object, coords);
}

static inline Coords GET_RESTORE_COORDS(const void *object) {
    return POSITION_HANDLER(object)->restore_coords;
}

static inline void SET_RESTORE_COORDS(void *object, Coords coords) {
    POSITION_HANDLER(object)->restore_coords = coords;
}
#endif
