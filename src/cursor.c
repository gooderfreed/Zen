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


Cursor init_cursor(Objects start_object, Coords start_coords) {
    Cursor cursor = {
        .coords = start_coords,
        .cards = {
            .container = {0},
            .size = 0,
            .source = Unknown
        },
        .subject = start_object,
    };
    return cursor;
}

void print_cursor(Cursor *cursor, Screen *screen, void *target_struct) {
    Coords base_coords = {
        .x = cursor->coords.x * CARD_WIDTH + (CARD_WIDTH / 2), 
        .y = BORDER_OFFSET_Y + CARD_HEIGHT
    };

    ObjectInterfaces *interfaces = (ObjectInterfaces*)target_struct;
    
    if (interfaces->capabilities.is_interactable) {
        interfaces->interactable->place_cursor(target_struct, cursor->coords, &base_coords);
    }

    if (base_coords.y < SCREEN_HEIGHT && base_coords.x < SCREEN_WIDTH) {
        screen->data[base_coords.y][base_coords.x] = '/';
        screen->data[base_coords.y][base_coords.x + 1] = '\\';
    }
}

