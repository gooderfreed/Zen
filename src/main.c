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

// deck

// deck



// card

// card



// field

// field



// cursor

// cursor


int main(void) {
    setlocale(LC_ALL, "");
    clear();

    Deck deck = generate_deck();
    Field field;
    Cursor cursor = init_cursor(&deck, &field, NULL);
    Screen screen = init_screen();

    init_field(field, &deck);

    add_borders(&screen, 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, fat_border);
    add_separator(&screen, DECK_OFFSET + BORDER_OFFSET_Y - 1, 0, fat_border);

    move_cursor(&cursor, 1, 0);

    print_field(&screen, &field, &cursor);
    print_cursor(&screen, &cursor);
    print_screen(&screen);
}

