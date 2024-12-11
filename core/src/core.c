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

/*
 * Core engine implementation
 * Contains main game loop logic and object management
 */
#include "../inc/core.h"

/*
 * Initialize core engine with provided components
 * Validates all interfaces before returning
 */
Core init_core(Map *map, Cursor *cursor, Screen *screen) {
    Core core = {
        .screen = screen,
        .map = map,
        .cursor = cursor,
    };

    core_validate_interfaces(&core);

    return core;
}

/*
 * Handle cursor movement request in current object
 * Moves cursor if target object is interactable and allows movement
 */
void core_local_move(Core *core, Coords move) {
    Cursor *cursor = core->cursor;
    if (!INTERACTABLE(cursor->subject)) return;

    Coords coords = {.x = cursor->coords.x, .y = cursor->coords.y};
    MOVE(cursor->subject, &coords, move);

    // Update cursor position only if movement was successful
    if (coords.x != cursor->coords.x || coords.y != cursor->coords.y) {
        cursor->coords = coords;
    }
}

/*
 * Handle action on current cursor position
 * Implements main game logic for card manipulation:
 * - Button handling
 * - Card selection
 * - Card movement between objects
 */
void core_action(Core *core) {
    Cursor *cursor = core->cursor;

    // Handle button press if cursor is on a button
    if (is_button(cursor->subject, cursor->coords)) {
        handle_button(cursor->subject, cursor->coords);
        core_update(core);
        return;
    }

    // Handle card operations
    if (cursor->cards->size != 0) {
        Card *first_card = cursor->cards->container[0];

        // Toggle card selection if clicking on the same card
        if (cursor->subject == cursor->cards->source &&
            IS_SAME_CARD(cursor->subject, cursor->coords, first_card)) {
            SELECT_CARDS(cursor->subject, cursor->coords, cursor->cards);
        }
        // Move cards if source can give and target can take
        else if (CAN_GIVE_CARDS(cursor->cards->source) && 
                 CAN_TAKE_CARDS(cursor->subject) &&
                 CAN_PLACE_CARDS(cursor->subject, cursor->coords, cursor->cards)) {
            GET_CARDS(cursor->cards->source, cursor->cards);
            PLACE_CARDS(cursor->subject, cursor->coords, cursor->cards);
        }
    }
    // Select cards if no cards are currently selected
    else {
        if (CAN_GIVE_CARDS(cursor->subject)) {
            SELECT_CARDS(cursor->subject, cursor->coords, cursor->cards);
        }
    }
    core_update(core);
}

/*
 * Update screen with current game state
 * Draws all visible objects and cursor
 */
void core_update_screen(Core *core) {
    // Draw all objects on map
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            void *target_struct = core->map->objects[y][x].object;
            if (!target_struct) continue;
            
            if (DRAWABLE(target_struct)) {
                DRAW(target_struct, core->screen, core->cursor);
            }
        }
    }

    // Draw cursor and update screen
    print_cursor(core->cursor, core->screen);
    print_screen(core->screen);
}

/*
 * Handle global map movement
 * Saves current position, moves map and restores position for new object
 */
void core_global_move(Core *core, Coords move) {
    // Save current position if object supports it
    if (POSITIONABLE(core->cursor->subject)) {
        SAVE_POSITION(core->cursor->subject, core->cursor->coords);
    }

    // Move map and get new object
    map_move(core->map, move);
    MapObject target = map_get_current_object(core->map);

    // Update cursor for new object
    if (target.object) {
        core->cursor->subject = target.object;
        if (POSITIONABLE(core->cursor->subject)) {
            RESTORE_POSITION(core->cursor->subject, &core->cursor->coords);
        }
        else {
            core->cursor->coords = target.default_coords;
        }
    }
}

/*
 * Free all dynamic objects on map
 * Called during shutdown to prevent memory leaks
 */
void core_free(Core *core) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            MapObject object = core->map->objects[y][x];
            if (!object.object) continue;

            if (DYNAMIC(object.object)) {
                FREE(object.object);
            }
        }
    }
}

/*
 * Update all objects on map
 * Calls update for each object
 */
void core_update(Core *core) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            MapObject object = core->map->objects[y][x];
            if (!object.object) continue;

            if (UPDATEABLE(object.object)) {
                UPDATE(object.object);
            }
            //     ObjectInterfaces *interfaces = (ObjectInterfaces *)object.object;
            //     interfaces->updateable->update(object.object, interfaces->updateable->context);
            // }
        }
    }
}

/*
 * Validate interfaces of all objects on map
 * Ensures all objects have correctly implemented required interfaces
 * Exits program if validation fails
 */
void core_validate_interfaces(Core *core) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            MapObject object = core->map->objects[y][x];
            if (!object.object) continue;
            
            ObjectInterfaces *interfaces = (ObjectInterfaces*)object.object;
            if (!validate_object_interfaces(interfaces)) {
                exit(1);
            }
        }
    }
}
