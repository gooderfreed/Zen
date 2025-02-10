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

static inline InputHandler *INPUT_HANDLER(const void *object) {
    return GET_INTERFACES(object)->input_handler;
}

static inline bool IS_INPUT_HANDLER(const void *object) {
    return GET_INTERFACES(object)->capabilities.requires_input;
}

static inline bool HANDLE_INPUT(void *object, Core *core, wint_t key) {
    return INPUT_HANDLER(object)->handle_input(object, core, key);
}

#endif
