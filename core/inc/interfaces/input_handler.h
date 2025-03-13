#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include "object_interfaces.h"

/*
 * InputHandler - Interface for objects that can handle input
 * Provides input handling capabilities
 */
typedef struct InputHandler {
    bool (*handle_input)(void *self, Core *core, wint_t key);
} InputHandler;

static inline InputHandler *GET_INPUT_HANDLER(const void *object) {
    return GET_INTERFACES(object)->input_handler;
}

static inline bool IS_INPUT_HANDLER(const void *object) {
    return GET_INTERFACES(object)->capabilities.requires_input;
}

static inline bool HANDLE_INPUT(void *object, Core *core, wint_t key) {
    return GET_INPUT_HANDLER(object)->handle_input(object, core, key);
}

#define INPUT_HANDLER_FULL(arena, object, _handle_input)                                                        \
    do {                                                                                                        \
        if (!IS_INPUT_HANDLER(object))                                                                          \
            GET_INTERFACES(object)->capabilities.requires_input = true;                                         \
        InputHandler *input_handler = GET_INPUT_HANDLER(object);                                                \
        if (input_handler == NULL) {                                                                            \
            GET_INTERFACES(object)->input_handler = (InputHandler *)arena_alloc(arena, sizeof(InputHandler));   \
            input_handler = GET_INPUT_HANDLER(object);                                                          \
        }                                                                                                       \
        input_handler->handle_input = _handle_input;                                                            \
    } while (0)

#define INPUT_HANDLER(_handle_input) \
    INPUT_HANDLER_FULL(cur_arena, cur_object, _handle_input)

#endif
