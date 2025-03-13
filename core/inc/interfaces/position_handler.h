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

#define POSITION_HANDLER_FULL(arena, object, _save_current_pos, _restore_pos)                                            \
    do {                                                                                                                 \
        if (!IS_POSITIONABLE(object))                                                                                \
            GET_INTERFACES(object)->capabilities.is_positionable = true;                                                 \
        PositionHandler *position_handler = POSITION_HANDLER(object);                                                    \
        if (position_handler == NULL) {                                                                                  \
            GET_INTERFACES(object)->position_handler = (PositionHandler *)arena_alloc(arena, sizeof(PositionHandler));   \
            position_handler = POSITION_HANDLER(object);                                                                 \
        }                                                                                                                \
        position_handler->restore_pos = _restore_pos;                                                                    \
        position_handler->save_current_pos = _save_current_pos;                                                          \
    } while (0)

#define POSITION_HANDLER(_restore_pos, _save_current_pos) \
    POSITION_HANDLER_FULL(cur_arena, cur_object, _restore_pos, _save_current_pos)

#endif
