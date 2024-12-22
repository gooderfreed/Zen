#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include "object_interfaces.h"

/*
 * ButtonHandler - Interface for objects that can handle buttons
 * Provides button position and handling capabilities
 */
#ifndef BUTTON_HANDLER_SIZE
    #define BUTTON_HANDLER_SIZE 1
#endif

typedef struct {
    Coords coords;
    void *context;
    void (*on_click)(void *self, void *context);
} Button;

typedef struct ButtonHandler {
    #ifndef BUTTON_HANDLER_DYNAMIC
        Button *buttons[BUTTON_HANDLER_SIZE];
    #else
        Button **buttons;
        int length;
    #endif
    int buttons_count;
} ButtonHandler;

#ifndef CUSTOM_BUTTON_HANDLER_IMPL
    #ifdef BUTTON_HANDLER_DYNAMIC
        Button **buttons_init(int length)
    #endif
    void add_button(ButtonHandler *button_handler, Button *button);
    void remove_button(ButtonHandler *button_handler, int index);
    void set_button_context(ButtonHandler *button_handler, int index, void *context);
    Button *get_button(ButtonHandler *button_handler, int index);
    bool is_button(void *object, Coords coords);
    Button *get_button_by_coords(void *object, Coords coords);
    void handle_button(void *object, Coords coords);
#endif

// Button Handler macros
static inline ButtonHandler *BUTTON_HANDLER(const void *object) {
    return GET_INTERFACES(object)->button_handler;
}

static inline bool HAVE_BUTTONS(const void *object) {
    return GET_INTERFACES(object)->capabilities.have_buttons && BUTTON_HANDLER(object)->buttons_count > 0;
}

// Button related macros
static inline void SET_BUTTON_CONTEXT(void *object, int index, void *context) {
    set_button_context(BUTTON_HANDLER(object), index, context);
}


#endif
