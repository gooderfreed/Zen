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
void select_card(Cursor *cursor) {
    Objects target = cursor->subject;
    void *target_struct = cursor->objects[target];
    ObjectInterface target_interface = *cursor->interfaces[target];

    if (cursor->card) {
        Objects source = cursor->card->object;
        void *source_struct = cursor->objects[source];
        ObjectInterface source_interface = *cursor->interfaces[source];

        bool same_position = (cursor->subject == source) &&
                             (cursor->coords.x == cursor->card->coords.x) &&
                             (cursor->coords.y == cursor->card->coords.y);

        if (same_position) {
            target_interface.select_card(target_struct, cursor->coords);
            cursor->card = NULL;
        }
        else if (target_interface.place_card && source_interface.get_card &&
                 target_interface.can_place(target_struct, cursor->card, cursor->coords)) {
            Card *card = source_interface.get_card(source_struct, cursor);
            if (card) {
                target_interface.place_card(target_struct, card, cursor->coords);
                cursor->card = NULL;
            }
        }
    }
    else {
        cursor->card = target_interface.select_card(target_struct, cursor->coords);
    }
}
// cursor



// screen
void update_screen(Screen *screen, Field *field, Cursor *cursor, Deck *deck) {
    (void) deck;
    print_field(screen, field, cursor);
    print_cursor(screen, cursor);
    print_screen(screen);
}
// screen



void set_noncanonical_mode(void) {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);

    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void restore_terminal_settings(void) {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);

    term.c_lflag |= ICANON | ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

int main(void) {
    setlocale(LC_ALL, "");
    clear();

    Deck   deck   = generate_deck();
    Field  field  = init_field(&deck);
    Cursor cursor = init_cursor(&deck, &field, NULL);
    Screen screen = init_screen();

    add_borders(&screen, 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, fat_border);
    add_separator(&screen, DECK_OFFSET + BORDER_OFFSET_Y - 1, 0, fat_border);

    update_screen(&screen, &field, &cursor, &deck);

    set_noncanonical_mode();
    bool need_screen_update;
    while (true) {
        wint_t ch = getwchar();
        // wprintf(L"key: %i       \n", ch);
        need_screen_update = true;
        switch (ch) {
            case L'q': case L'й': case KEY_ESC:
                clear();
                restore_terminal_settings();
                exit(0);
            case L'a': case L'ф':
                move_cursor(&cursor, -1, 0);
                break;
            case L'd': case L'в':
                move_cursor(&cursor, 1, 0);
                break;
            case L'w': case L'ц':
                move_cursor(&cursor, 0, -1);
                break;
            case L's': case L'ы': case L'і':
                move_cursor(&cursor, 0, 1);
                break;
            case L' ': case L'\n':
                // wprintf(L"asdasdasdsdasda\n");
                select_card(&cursor);
                break;
            default:
                need_screen_update = false;
        }

        if (need_screen_update) {
            update_screen(&screen, &field, &cursor, &deck);
            need_screen_update = false;
        }
    }
}

