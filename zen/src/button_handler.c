/*
 * Button handling implementation
 * Manages button creation, removal, and click handling
 */
#include "../inc/zen.h"

/*
 * Check if a button exists at given coordinates
 * Returns true if a button is found, false otherwise
 */
bool is_button(void *object, Coords coords) {
    if (!HAVE_BUTTONS(object)) return false;
    ButtonHandler *button_handler = GET_BUTTON_HANDLER(object);
    for (int i = 0; i < button_handler->button_groups_count; i++) {
        ButtonGroup *button_group = button_handler->button_groups[i];
        for (int j = 0; j < button_group->length; j++) {
            Button *button = button_group->buttons[j];
            if (button->coords.x == coords.x && button->coords.y == coords.y) {
                return true;
            }
        }
    }
    return false;
}

/*
 * Get a button by its coordinates
 * Returns the button if found, NULL otherwise
 */
Button *get_button_by_coords(void *object, Coords coords) {
    if (!HAVE_BUTTONS(object)) return NULL;
    ButtonHandler *button_handler = GET_BUTTON_HANDLER(object);
    for (int i = 0; i < button_handler->button_groups_count; i++) {
        ButtonGroup *button_group = button_handler->button_groups[i];
        for (int j = 0; j < button_group->length; j++) {
            Button *button = button_group->buttons[j];
            if (button->coords.x == coords.x && button->coords.y == coords.y) {
                return button;
            }
        }
    }
    return NULL;
}

/*
 * Handle button click
 * Retrieves the button at given coordinates and calls its on_click function
 */
void handle_button(void *object, Coords coords) {
    Button *button = get_button_by_coords(object, coords);
    if (button != NULL) {
        button->on_click(object, button->context);
    }
}

/*
 * Set the context for a button
 * Sets the context for the button with the specified name
 */
void set_button_context(ButtonHandler *button_handler, char *name, void *context) {
    if (!button_handler || !name) return;

    for (int i = 0; i < button_handler->button_groups_count; i++) {
        ButtonGroup *button_group = button_handler->button_groups[i];
        for (int j = 0; j < button_group->length; j++) {
            Button *button = button_group->buttons[j];
            if (name[0] == button->name[0] && strcmp(button->name, name) == 0) {
                button->context = context;
            }
        }
    }
}

