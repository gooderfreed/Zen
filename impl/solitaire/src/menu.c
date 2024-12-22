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

#include "../inc/solitare.h"

/*
 * Print menu
 * Draws the menu on the screen
 */
static void print_menu(const void *menu_pointer, Screen *screen, const Cursor *cursor) {
    (void)menu_pointer;
    (void)cursor;

    int text_offset = 1;
    int text_y = 18;

    Color text_color = COLOR_BOLD;
    Color background_color = COLOR_BLACK;

    insert_text(screen, text_y + 0, text_offset, ".dP\"Y8  dP\"Yb  88     88 888888    db    88 88\"\"Yb 88888", text_color, background_color);
    insert_text(screen, text_y + 1, text_offset, "`Ybo.\" dP   Yb 88     88   88     dPYb   88 88__dP 88__ ",    text_color, background_color);
    insert_text(screen, text_y + 2, text_offset, "o.`Y8b Yb   dP 88  .o 88   88    dP__Yb  88 88\"Yb  88\"\" ",  text_color, background_color);
    insert_text(screen, text_y + 3, text_offset, "8bodP'  YbodP  88ood8 88   88   dP\"\"\"\"Yb 88 88  Yb 88888", text_color, background_color);

    insert_text(screen, text_y + 8,  SCREEN_WIDTH/2 - 6, "  Start",    text_color, background_color);
    insert_text(screen, text_y + 9,  SCREEN_WIDTH/2 - 6, "  Options",  text_color, background_color);
    insert_text(screen, text_y + 10, SCREEN_WIDTH/2 - 6, "  Controls", text_color, background_color);
    insert_text(screen, text_y + 11, SCREEN_WIDTH/2 - 6, "  Exit",     text_color, background_color);
}

/*
 * Place cursor in menu
 * Places the cursor in the menu
 */
static void place_cursor_in_menu(const void *menu_pointer, const Coords cursor_coords, Coords *target_coords) {
    (void)menu_pointer;
    (void)cursor_coords;
    target_coords->y = 26 + cursor_coords.y;
    target_coords->x = SCREEN_WIDTH/2 - 6;
}

/*
 * Move in menu
 * Moves the cursor in the menu
 */
static void move_in_menu(const void *menu_pointer, Coords *coords, const Coords delta) {
    (void)menu_pointer;
    if (delta.x != 0) return;
    short new_y = coords->y + delta.y;
    coords->y = (new_y + 4) % 4;
}

/*
 * Get default coords
 * Gets the default coords of the menu
 */
static Coords get_default_coords(const void *menu_pointer) {
    (void)menu_pointer;
    return (Coords) {.x = 0, .y = 0};
}

/*
 * Get cursor config
 * Gets the cursor config of the menu
 */
static CursorConfig get_cursor_config_in_menu(const void *menu_pointer, const Coords cursor_coords) {
    (void)menu_pointer;
    (void)cursor_coords;
    return (CursorConfig) {.type = CURSOR_RIGHT_SLIM};
}

/*
 * On start click
 * Handles the start button click
 */
static void on_start_click(void *menu, void *context) {
    (void)menu;
    Core *core = (Core *)context;
    core_change_layer(core, 1);
}

/*
 * On exit click
 * Handles the exit button click
 */
static void on_exit_click(void *menu, void *context) {
    (void)menu;
    Core *core = (Core *)context;
    clear();
    show_cursor();
    core_free(core);
    restore_terminal_settings();
    exit(0);
}

/*
 * On options click
 * Handles the options button click
 */
static void on_options_click(void *menu, void *context) {
    (void)menu;
    (void)context;
}

/*
 * On controls click
 * Handles the controls button click
 */
static void on_controls_click(void *menu, void *context) {
    (void)menu;
    (void)context;
}

/*
 * Init menu
 * Initializes the menu
 */
Menu init_menu(void) {
    Menu menu = {0};

    static const Drawable drawable = {
        .print = print_menu
    };

    static const Interactable interactable = {
        .place_cursor        = place_cursor_in_menu,
        .move                = move_in_menu,
        .get_default_coords  = get_default_coords,
        .get_cursor_config   = get_cursor_config_in_menu
    };

    static Button start_button = {
        .coords = {.x = 0, .y = 0},
        .on_click = on_start_click
    };

    static Button options_button = {
        .coords = {.x = 0, .y = 1},
        .on_click = on_options_click
    };

    static Button controls_button = {
        .coords = {.x = 0, .y = 2},
        .on_click = on_controls_click
    };

    static Button exit_button = {
        .coords = {.x = 0, .y = 3},
        .on_click = on_exit_click
    };

    static ButtonHandler button_handler = {
        .buttons_count = 4,
        .buttons = {
            [0] = &start_button,
            [1] = &options_button,
            [2] = &controls_button,
            [3] = &exit_button
        }
    };

    menu.interfaces = (ObjectInterfaces) {
        .name = "Menu",
        .capabilities = {
            .is_drawable     = true,
            .have_buttons    = true,
            .is_interactable = true,
        },
        .drawable       = &drawable,
        .interactable   = &interactable,
        .button_handler = &button_handler
    };
    
    return menu;
}
