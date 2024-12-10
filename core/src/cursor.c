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
 * Cursor implementation
 * Handles cursor state and visualization
 */
#include "../inc/core.h"


/*
 * Initialize cursor with starting object and position
 * Returns cursor structure ready for use
 */
Cursor init_cursor(void *start_object, Coords start_coords, Container *cursor_cards) {
    Cursor cursor = {
        .coords = start_coords,
        .cards = cursor_cards,
        .subject = start_object,
    };

    return cursor;
}

/*
 * Print cursor on screen
 * Handles cursor visualization based on current state and position
 */
void print_cursor(Cursor *cursor, Screen *screen) {
    // Get base coordinates for cursor
    Coords base_coords = {
        .x = cursor->coords.x * CARD_WIDTH + (CARD_WIDTH / 2), 
        .y = CARD_HEIGHT
    };

    // If subject is interactable, let it place the cursor
    if (INTERACTABLE(cursor->subject)) {
        PLACE_CURSOR(cursor->subject, cursor->coords, &base_coords);
    }
    
    // Draw cursor symbol at calculated position
    if (base_coords.y < SCREEN_HEIGHT && base_coords.x < SCREEN_WIDTH) {
        screen->data[base_coords.y][base_coords.x] = '/';
        screen->data[base_coords.y][base_coords.x + 1] = '\\';
    }
}

