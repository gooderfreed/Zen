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

// #define DEBUG
#ifndef DEBUG
static void set_noncanonical_mode(void) {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);

    term.c_lflag &= (tcflag_t)~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

static void restore_terminal_settings(void) {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);

    term.c_lflag |= ICANON | ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}
#endif

int main(void) {
    setlocale(LC_ALL, "");
    clear();

    Deck   deck   = generate_deck();
    Field  field  = init_field(&deck); // TODO: change deck get_card method
    Stock  stock  = init_stock();
    Screen screen = init_screen();

    #ifndef DEBUG
    Map map = {
        .objects = {
            {
                {&deck, DECK_DEFAULT_COORDS}, {&field, FIELD_DEFAULT_COORDS}, {&stock, STOCK_DEFAULT_COORDS}
            }
        },
        .global_coords = (Coords){.x = 1, .y = 0}
    };

    MapObject object = map_get_current_object(&map);
    Cursor    cursor = init_cursor(object.object, object.default_coords);
    Core      core   = init_core(&map, &cursor, &screen);

    add_borders(&screen, 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH, fat_border);
    add_separator(&screen, DECK_OFFSET + BORDER_OFFSET_Y - 1, 0, fat_border);

    core_update_screen(&core);

    set_noncanonical_mode();
    bool need_screen_update;
    while (true) {
        wint_t ch = getwchar();
        // wprintf(L"key: %i       \n", ch);
        need_screen_update = true;
        switch (ch) {
            case L'q': case L'й': case KEY_ESC:
                clear();
                core_free(&core);
                restore_terminal_settings();
                exit(0);
            case L'a': case L'ф': core_move(&core, CURSOR_LEFT);            break;
            case L'd': case L'в': core_move(&core, CURSOR_RIGHT);           break;
            case L'w': case L'ц': core_move(&core, CURSOR_UP);              break;
            case L's': case L'ы': case L'і': core_move(&core, CURSOR_DOWN); break;
            case KEY_SPACE: case KEY_ENTER: core_action(&core);             break;
            case KEY_CTRL_A: core_global_move(&core, CURSOR_LEFT);          break;
            case KEY_CTRL_D: core_global_move(&core, CURSOR_RIGHT);         break;
            default: need_screen_update = false;
        }

        if (need_screen_update) {
            core_update_screen(&core);
            need_screen_update = false;
        }
    }
    #endif
}
