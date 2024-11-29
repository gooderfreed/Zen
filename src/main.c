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



// screen
// screen



// core
Core init_core(Cursor *cursor, Screen *screen, Deck *deck, Field *field) {
    Core core = {
        .cursor = cursor,
        .screen = screen,
        .objects = {
            [Deck_enum] = deck,
            [Field_enum] = field,
            // [Stock_enum] = stock
        }
    };

    return core;
}

void core_move(Core *core, Coords move) {
    Cursor *cursor = core->cursor;
    Coords coords = {.x = cursor->coords.x, .y = cursor->coords.y};

    Objects target = cursor->subject;
    void *target_struct = core->objects[target];
    ObjectInterface target_interface = *((ObjectInterface*)target_struct);

    target_interface.move(target_struct, &coords, move);

    if (coords.x != cursor->coords.x || coords.y != cursor->coords.y) {
        wprintf(L"x: %i - y: %i", coords.x, coords.y);
        if (coords.x < 0) {
            cursor->subject = (Objects)(-coords.x) - 1;
            cursor->coords  = (Coords){0, 0, 0};
        }
        else {
            cursor->coords = coords;
        }
    }
}

void core_action(Core *core) {
    Cursor *cursor = core->cursor;
    Objects target = core->cursor->subject;
    void *target_struct = core->objects[target];
    ObjectInterface target_interface = *((ObjectInterface*)target_struct);

    if (cursor->cards.size != 0) {
        Objects source = cursor->cards.source;
        void *source_struct = core->objects[source];
        ObjectInterface source_interface = *((ObjectInterface*)source_struct);
        Card *first_card = core->cursor->cards.container[0];

        bool same_position = (cursor->subject == source) &&
                             (cursor->coords.x == first_card->coords.x) &&
                             (cursor->coords.y == first_card->coords.y);

        if (same_position) {
            target_interface.select_cards(target_struct, cursor->coords, &cursor->cards);
        }
        else if (target_interface.place_cards && source_interface.get_cards &&
                 target_interface.can_place(target_struct, cursor->coords, &cursor->cards)) {
            source_interface.get_cards(source_struct, &cursor->cards);
            target_interface.place_cards(target_struct, cursor->coords, &cursor->cards);
        }
    }
    else {
        target_interface.select_cards(target_struct, cursor->coords, &cursor->cards);
    }
}

void core_update_screen(Core *core) {
    for (Objects object = 0; object < OBJECTS_COUNT - 1; ++object) {
        void *target_struct = core->objects[object];
        ObjectInterface target_interface = *((ObjectInterface*)target_struct);
        if (target_interface.is_drawable) {
            target_interface.print(target_struct, core->screen, core->cursor);
        }
    }

    Objects target = core->cursor->subject;
    void *target_struct = core->objects[target];
    print_cursor(core->cursor, core->screen, target_struct);
    print_screen(core->screen);
}
// core


void set_noncanonical_mode(void) {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);

    term.c_lflag &= (tcflag_t)~(ICANON | ECHO);
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
    Field  field  = init_field(&deck); // TODO: change deck get_card method
    Cursor cursor = init_cursor(Field_enum, (Coords){0, 0, 0});
    Screen screen = init_screen();

    Core core = init_core(&cursor, &screen, &deck, &field);



    add_borders(&screen, 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, fat_border);
    add_separator(&screen, DECK_OFFSET + BORDER_OFFSET_Y - 1, 0, fat_border);

    core_update_screen(&core);


    set_noncanonical_mode();
    bool need_screen_update;
    while (true) {
        wint_t ch = getwchar();
        wprintf(L"key: %i       \n", ch);
        need_screen_update = true;
        switch (ch) {
            case L'q': case L'й': case KEY_ESC:
                clear();
                restore_terminal_settings();
                exit(0);
            case L'a': case L'ф': core_move(&core, CURSOR_LEFT);
                break;
            case L'd': case L'в': core_move(&core, CURSOR_RIGHT);
                break;
            case L'w': case L'ц': core_move(&core, CURSOR_UP);
                break;
            case L's': case L'ы': case L'і': core_move(&core, CURSOR_DOWN);
                break;
            case L' ': case L'\n': core_action(&core);
                break;
            case 1:
                break;

            case 4:
                break;

            default: need_screen_update = false;
        }


        if (need_screen_update) {
            core_update_screen(&core);
            need_screen_update = false;
        }
    }
}

