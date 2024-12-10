/*
 * Copyright 2024 Cheban Sergiy
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#ifndef CUSTOM_BUTTON_HANDLER_IMPL

/*
 * Button handling implementation
 * Manages button creation, removal, and click handling
 */
#include "../inc/core.h"

#ifdef BUTTON_HANDLER_DYNAMIC
    /*
     * Initialize button handler
     * Allocates memory for button handler and sets initial size
     */
    inline Button **buttons_init(int length) {
        return (Button **)malloc(length * sizeof(Button *));
    }

    /*
    * Add a button to the button handler
    * Adds the button to the button handler's array and increments the count
    */
    void add_button(ButtonHandler *button_handler, Button *button) {
        if (!button_handler || !button) return;
        if (button_handler->buttons_count >= button_handler->length) return;
        button_handler->buttons[button_handler->buttons_count] = button;
        button_handler->buttons_count++;
    }
#else
    /*
    * Add a button to the button handler
    * Adds the button to the button handler's array and increments the count
    */
    void add_button(ButtonHandler *button_handler, Button *button) {
        if (!button_handler || !button) return;
        if (button_handler->buttons_count >= BUTTON_HANDLER_SIZE) return;
        button_handler->buttons[button_handler->buttons_count] = button;
        button_handler->buttons_count++;
    }
#endif

/*
 * Check if a button exists at given coordinates
 * Returns true if a button is found, false otherwise
 */
bool is_button(void *object, Coords coords) {
    if (!HAVE_BUTTONS(object)) return false;
    ButtonHandler *button_handler = BUTTON_HANDLER(object);
    for (int i = 0; i < button_handler->buttons_count; i++) {
        Button *button = button_handler->buttons[i];
        if (button->coords.x == coords.x && button->coords.y == coords.y) {
            return true;
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
    ButtonHandler *button_handler = BUTTON_HANDLER(object);
    for (int i = 0; i < button_handler->buttons_count; i++) {
        Button *button = button_handler->buttons[i];
        if (button->coords.x == coords.x && button->coords.y == coords.y) {
            return button;
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
 * Remove a button from the button handler
 * Sets the button at the specified index to NULL and decrements the count
 */
void remove_button(ButtonHandler *button_handler, int index) {
    if (index < 0 || index >= button_handler->buttons_count) return;
    button_handler->buttons[index] = NULL;
    button_handler->buttons_count--;
}

/*
 * Set the context for a button
 * Sets the context for the button at the specified index
 */
void set_button_context(ButtonHandler *button_handler, int index, void *context) {
    if (index < 0 || index >= button_handler->buttons_count) return;
    button_handler->buttons[index]->context = context;
}

/*
 * Get a button from the button handler
 * Returns the button at the specified index
 */
Button *get_button(ButtonHandler *button_handler, int index) {
    if (index < 0 || index >= button_handler->buttons_count) return NULL;
    return button_handler->buttons[index];
}

#endif
