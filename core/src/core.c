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
    if (!INTERACTABLE(cursor->subject)) return;

    Coords coords = {.x = cursor->coords.x, .y = cursor->coords.y};
    MOVE(cursor->subject, &coords, move);

    if (coords.x != cursor->coords.x || coords.y != cursor->coords.y) {
        cursor->coords = coords;
    }
}

void core_action(Core *core) {
    Cursor *cursor = core->cursor;

    if (IS_BUTTON(cursor->subject, cursor->coords)) {
        HANDLE_BUTTON(cursor->subject, cursor->coords);
        return;
    }

    if (cursor->cards.size != 0) {
        Card *first_card = cursor->cards.container[0];

        if (cursor->subject == cursor->cards.source && IS_SAME_CARD(cursor->subject, cursor->coords, first_card)) {
            SELECT_CARDS(cursor->subject, cursor->coords, &cursor->cards);
        }
        else if (CAN_GIVE_CARDS(cursor->cards.source) && CAN_TAKE_CARDS(cursor->subject)) {
            GET_CARDS(cursor->cards.source, &cursor->cards);
            PLACE_CARDS(cursor->subject, cursor->coords, &cursor->cards);
        }
    }
    else {
        SELECT_CARDS(cursor->subject, cursor->coords, &cursor->cards);
    }
}

void core_update_screen(Core *core) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            void *target_struct = core->map->objects[y][x].object;
            if (!target_struct) continue;
            
            if (DRAWABLE(target_struct)) {
                DRAW(target_struct, core->screen, core->cursor);
            }
        }
    }

    print_cursor(core->cursor, core->screen);
    print_screen(core->screen);
}

void core_global_move(Core *core, Coords move) {
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

            if (DYNAMIC(target_struct)) {
                FREE(target_struct);
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
