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

// map
Map init_map(Coords start_global_coords, Deck *deck, Field *field, Stock *stock) {
    Map map = {
        .objects = {
            {
                {deck, DECK_DEFAULT_COORDS}, {field, FIELD_DEFAULT_COORDS}, {stock, STOCK_DEFAULT_COORDS}
            }
        },
        .global_coords = start_global_coords
    };
    return map;
}

void map_move(Map *map, Coords move) {
    if (move.x == 0 && move.y == 0) return;

    Coords new_coords = {
        .x = map->global_coords.x + move.x,
        .y = map->global_coords.y + move.y,
    };

    if (new_coords.x < 0 || new_coords.x >= MAP_WIDTH ||
        new_coords.y < 0 || new_coords.y >= MAP_HEIGHT) {
        return;
    }

    MapObject object = map->objects[new_coords.y][new_coords.x];
    if (!object.object) return;
    if (!((ObjectInterfaces*)object.object)->capabilities.is_interactable) return;

    map->global_coords = new_coords;
}

MapObject map_get_current_object(Map *map) {
    return map->objects[map->global_coords.y][map->global_coords.x];
}
// map

// core
Core init_core(Map *map, Cursor *cursor, Screen *screen) {
    Core core = {
        .screen = screen,
        .map = map,
        .cursor = cursor,
    };

    return core;
}

void core_move(Core *core, Coords move) {
    Cursor *cursor = core->cursor;
    Coords coords = {.x = cursor->coords.x, .y = cursor->coords.y};

    ObjectInterfaces *interfaces = (ObjectInterfaces*)cursor->subject;

    if (!interfaces->capabilities.is_interactable) return;

    interfaces->interactable->move(cursor->subject, &coords, move);

    if (coords.x != cursor->coords.x || coords.y != cursor->coords.y) {
        wprintf(L"x: %i - y: %i", coords.x, coords.y);
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

        bool same_position = (cursor->subject == cursor->cards.source) &&
                           (cursor->coords.x == first_card->coords.x) &&
                           (cursor->coords.y == first_card->coords.y);

        if (same_position) {
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
    wprintf(L"move: %i             \n", move.x);
    map_move(core->map, move);

    MapObject target = map_get_current_object(core->map);
    if (target.object) {
        core->cursor->subject = target.object;
        core->cursor->coords = target.default_coords;
    }
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

static bool validate_drawable(const void *interface) {
    const Drawable *drawable = interface;
    if (!drawable) {
        wprintf(L"Error: Drawable interface is NULL\n");
        return false;
    }

    if (!drawable->print) {
        wprintf(L"Error: Drawable interface is missing print function\n");
        return false;
    }
    return true;
}

static bool validate_interactable(const void *interface) {
    const Interactable *interactable = interface;
    if (!interactable) {
        wprintf(L"Error: Interactable interface is NULL\n");
        return false;
    }

    if (!interactable->move || !interactable->place_cursor) {
        wprintf(L"Error: Interactable interface is missing required functions\n");
        return false;
    }
    return true;
}

static bool validate_card_handler(const void *interface) {
    const CardHandler *handler = interface;
    if (!handler) {
        wprintf(L"Error: CardHandler interface is NULL\n");
        return false;
    }

    if (handler->can_give_cards) {
        if (!handler->get_cards || !handler->select_cards) {
            wprintf(L"Error: Object declares can_give_cards but required functions are missing\n");
            return false;
        }
    }

    if (handler->can_take_cards) {
        if (!handler->place_cards || !handler->can_place) {
            wprintf(L"Error: Object declares can_take_cards but required functions are missing\n");
            return false;
        }
    }
    return true;
}

static bool validate_button_handler(const void *interface) {
    const ButtonHandler *handler = interface;
    if (!handler) {
        wprintf(L"Error: ButtonHandler interface is NULL\n");
        return false;
    }
    if (!handler->is_button_position || !handler->handle_button) {
        wprintf(L"Error: ButtonHandler interface is missing required functions\n");
        return false;
    }
    return true;
}

bool validate_object_interfaces(const ObjectInterfaces *interfaces) {
    InterfaceValidator validators[] = {
        {
            .capability_flag = interfaces->capabilities.is_drawable,
            .interface = interfaces->drawable,
            .validate = validate_drawable
        },
        {
            .capability_flag = interfaces->capabilities.is_interactable,
            .interface = interfaces->interactable,
            .validate = validate_interactable
        },
        {
            .capability_flag = interfaces->capabilities.can_hold_cards,
            .interface = interfaces->card_handler,
            .validate = validate_card_handler
        },
        {
            .capability_flag = interfaces->capabilities.have_buttons,
            .interface = interfaces->button_handler,
            .validate = validate_button_handler
        }
    };

    for (size_t i = 0; i < sizeof(validators) / sizeof(validators[0]); i++) {
        const InterfaceValidator *v = &validators[i];
        
        if (v->capability_flag) {
            if (!v->validate(v->interface)) {
                return false;
            }
        }
    }

    return true;
}

int main(void) {
    setlocale(LC_ALL, "");
    clear();


    Deck   deck   = generate_deck();
    Field  field  = init_field(&deck); // TODO: change deck get_card method
    Stock  stock  = init_stock();
    Screen screen = init_screen();
    Map    map = init_map((Coords){.x = 1, .y = 0}, &deck, &field, &stock);
    MapObject object = map_get_current_object(&map);
    Cursor cursor = init_cursor(object.object, object.default_coords);
    Core   core   = init_core(&map, &cursor, &screen);

    if (!validate_object_interfaces(&deck.interfaces) || 
        !validate_object_interfaces(&field.interfaces)) {
        exit(1);
    }

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
                core_global_move(&core, CURSOR_LEFT);
                break;

            case 4:
                core_global_move(&core, CURSOR_RIGHT);
                break;

            default: need_screen_update = false;
        }


        if (need_screen_update) {
            core_update_screen(&core);
            need_screen_update = false;
        }
    }
}

