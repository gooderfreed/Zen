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

#include "../inc/core.h"

Core init_core(Map *map, Cursor *cursor, Screen *screen) {
    Core core = {
        .screen = screen,
        .map = map,
        .cursor = cursor,
    };

    core_validate_interfaces(&core);

    return core;
}

void core_move(Core *core, Coords move) {
    Cursor *cursor = core->cursor;
    Coords coords = {.x = cursor->coords.x, .y = cursor->coords.y};

    ObjectInterfaces *interfaces = (ObjectInterfaces*)cursor->subject;

    if (!interfaces->capabilities.is_interactable) return;

    interfaces->interactable->move(cursor->subject, &coords, move);

    if (coords.x != cursor->coords.x || coords.y != cursor->coords.y) {
        // wprintf(L"x: %i - y: %i", coords.x, coords.y);
        cursor->coords = coords;
    }
}

void core_action(Core *core) {
    Cursor *cursor = core->cursor;
    ObjectInterfaces *target_interfaces = (ObjectInterfaces*)cursor->subject;

    if (target_interfaces->capabilities.have_buttons && 
        target_interfaces->button_handler->is_button_position(cursor->subject, cursor->coords)) {
        target_interfaces->button_handler->handle_button(cursor->subject, cursor->coords);
        return;
    }

    if (!target_interfaces->capabilities.can_hold_cards) return;

    if (cursor->cards.size != 0) {
        ObjectInterfaces *source_interfaces = (ObjectInterfaces*)cursor->cards.source;
        Card *first_card = cursor->cards.container[0];

        if (cursor->subject == cursor->cards.source &&
            target_interfaces->card_handler->is_same_card(cursor->subject, cursor->coords, first_card)) {
            target_interfaces->card_handler->select_cards(cursor->subject, cursor->coords, &cursor->cards);
        }
        else if (source_interfaces->capabilities.can_hold_cards && 
                 target_interfaces->card_handler->can_place(cursor->subject, cursor->coords, &cursor->cards)) {
            source_interfaces->card_handler->get_cards(cursor->cards.source, &cursor->cards);
            target_interfaces->card_handler->place_cards(cursor->subject, cursor->coords, &cursor->cards);
        }
    }
    else {
        target_interfaces->card_handler->select_cards(cursor->subject, cursor->coords, &cursor->cards);
    }
}

void core_update_screen(Core *core) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            void *target_struct = core->map->objects[y][x].object;
            if (!target_struct) continue;
            
            ObjectInterfaces *interfaces = (ObjectInterfaces*)target_struct;
            if (interfaces->capabilities.is_drawable) {
                interfaces->drawable->print(target_struct, core->screen, core->cursor);
            }
        }
    }

    print_cursor(core->cursor, core->screen);
    print_screen(core->screen);
}

void core_global_move(Core *core, Coords move) {
    // wprintf(L"move: %i             \n", move.x);
    map_move(core->map, move);

    MapObject target = map_get_current_object(core->map);
    if (target.object) {
        core->cursor->subject = target.object;
        core->cursor->coords = target.default_coords;
    }
}

void core_free(Core *core) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            void *target_struct = core->map->objects[y][x].object;
            if (!target_struct) continue;

            ObjectInterfaces *interfaces = (ObjectInterfaces*)target_struct;
            if (interfaces->capabilities.is_dynamic) {
                interfaces->dynamic->free(target_struct);
            }
        }
    }
}

void core_validate_interfaces(Core *core) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            void *target_struct = core->map->objects[y][x].object;
            if (!target_struct) continue;
            
            ObjectInterfaces *interfaces = (ObjectInterfaces*)target_struct;
            if (!validate_object_interfaces(interfaces)) {
                exit(1);
            }
        }
    }
}
