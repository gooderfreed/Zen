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

#ifndef CORE_H
#define CORE_H

#include <stdbool.h>
#include <wchar.h>

// Base structures

// Coords
typedef struct Coords {
    short y;
    short x;
    short z;
} Coords;

// Card
#ifndef CARD_HEIGHT
    #define CARD_HEIGHT 1
#endif
#ifndef CARD_WIDTH
    #define CARD_WIDTH 1
#endif

#ifndef CARD_STRUCT
typedef struct Card {
    Coords coords;
} Card;
#endif

// Container
#ifndef CONTAINER_SIZE
    #define CONTAINER_SIZE 1
#endif

typedef struct CardsContainer {
    int size;
    void *source;
    struct Card *container[CONTAINER_SIZE];
} CardsContainer;

// Interfaces
typedef struct Screen Screen;
typedef struct Cursor Cursor;

typedef struct {
    void (*print)(void *, Screen *, const Cursor *);
} Drawable;

typedef struct {
    void (*place_cursor)(void *, Coords, Coords *);
    void (*move)(void *, Coords *, Coords);
} Interactable;

typedef struct {
    bool can_give_cards : 1;
    bool can_take_cards : 1;

    void (*select_cards)(void *, Coords, CardsContainer *);
    void (*get_cards)(void *, CardsContainer *);
    void (*place_cards)(void *, Coords, CardsContainer *);
    bool (*can_place)(void *, Coords, CardsContainer *);
    bool (*is_same_card)(void *, Coords, struct Card *);
} CardHandler;

typedef struct {
    bool (*is_button_position)(void *, Coords);
    void (*handle_button)(void *, Coords);
} ButtonHandler;

typedef struct {
    void (*free)(void *);
} Dynamic;

typedef struct ObjectInterfaces {
    struct {
        bool can_hold_cards  : 1;
        bool have_buttons    : 1;
        bool is_drawable     : 1;
        bool is_interactable : 1;
        bool is_dynamic      : 1;
    } capabilities;

    const CardHandler   *card_handler;
    const ButtonHandler *button_handler;
    const Drawable      *drawable;
    const Interactable  *interactable;
    const Dynamic       *dynamic;
} ObjectInterfaces;

// Получение интерфейса из объекта
#define GET_INTERFACE(object, iface) \
    (((ObjectInterfaces*)(object))->iface)

// Проверка capability
#define HAS_CAPABILITY(object, cap) \
    (((ObjectInterfaces*)(object))->capabilities.cap)

// Безопасное получение интерфейса с проверкой capability
#define SAFE_GET_INTERFACE(object, iface, cap) \
    (HAS_CAPABILITY(object, cap) ? GET_INTERFACE(object, iface) : NULL)

// Card Handler macros
#define CARD_HANDLER(object) \
    GET_INTERFACE(object, card_handler)

#define CAN_GIVE_CARDS(object) \
    (HAS_CAPABILITY(object, can_hold_cards) && CARD_HANDLER(object)->can_give_cards)

#define CAN_TAKE_CARDS(object) \
    (HAS_CAPABILITY(object, can_hold_cards) && CARD_HANDLER(object)->can_take_cards)

#define SELECT_CARDS(object, coords, container) \
    (CARD_HANDLER(object)->select_cards(object, coords, container))

#define IS_SAME_CARD(object, coords, card) \
    (CARD_HANDLER(object)->is_same_card(object, coords, card))

#define GET_CARDS(object, container) \
    (CARD_HANDLER(object)->get_cards(object, container))

#define PLACE_CARDS(object, coords, container) \
    (CARD_HANDLER(object)->place_cards(object, coords, container))

// Button Handler macros
#define BUTTON_HANDLER(object) \
    GET_INTERFACE(object, button_handler)

#define IS_BUTTON(object, coords) \
    (HAS_CAPABILITY(object, have_buttons) && BUTTON_HANDLER(object)->is_button_position(object, coords))

#define HANDLE_BUTTON(object, coords) \
    (BUTTON_HANDLER(object)->handle_button(object, coords))

// Drawable macros
#define DRAW_HANDLER(object) \
    GET_INTERFACE(object, drawable)

#define DRAWABLE(object) \
    (HAS_CAPABILITY(object, is_drawable))

#define DRAW(object, screen, cursor) \
    (DRAW_HANDLER(object)->print(object, screen, cursor))

// Dynamic macros
#define DYNAMIC_HANDLER(object) \
    GET_INTERFACE(object, dynamic)

#define DYNAMIC(object) \
    (HAS_CAPABILITY(object, is_dynamic))

#define FREE(object) \
    (DYNAMIC_HANDLER(object)->free(object))

// Interactable macros
#define INTERACT_HANDLER(object) \
    GET_INTERFACE(object, interactable)

#define INTERACTABLE(object) \
    (HAS_CAPABILITY(object, is_interactable))

#define MOVE(object, coords, delta) \
    (INTERACT_HANDLER(object)->move(object, coords, delta))

#define PLACE_CURSOR(object, coords, base_coords) \
    (INTERACT_HANDLER(object)->place_cursor(object, coords, base_coords))

// Screen
#ifndef SCREEN_HEIGHT
    #define SCREEN_HEIGHT 1
#endif
#ifndef SCREEN_WIDTH
    #define SCREEN_WIDTH 1
#endif

#define clear() wprintf(L"\033[H\033[J")
#define gotoxy(x,y) wprintf(L"\033[%d;%dH", (y), (x))
#define KEY_ESC L''
#define KEY_ENTER 10
#define KEY_SPACE 32
#define KEY_CTRL_A 1
#define KEY_CTRL_D 4

struct Screen {
    char *background[SCREEN_HEIGHT][SCREEN_WIDTH];
    wchar_t data[SCREEN_HEIGHT][SCREEN_WIDTH];
    char *foreground[SCREEN_HEIGHT][SCREEN_WIDTH];
};

// Cursor
#define CURSOR_UP    (Coords){.x = 0,  .y = -1}
#define CURSOR_DOWN  (Coords){.x = 0,  .y = 1}
#define CURSOR_LEFT  (Coords){.x = -1, .y = 0}
#define CURSOR_RIGHT (Coords){.x = 1,  .y = 0}
#define CURSOR_STAY  (Coords){.x = 0,  .y = 0}

struct Cursor {
    Coords coords;
    CardsContainer cards;
    void *subject;
};

// Map
#ifndef MAP_HEIGHT
    #define MAP_HEIGHT 1
#endif
#ifndef MAP_WIDTH
    #define MAP_WIDTH 1
#endif

typedef struct MapObject {
    void *object;
    Coords default_coords;
} MapObject;

typedef struct Map {
    MapObject objects[MAP_HEIGHT][MAP_WIDTH];
    Coords global_coords;
} Map;

// Core
typedef struct {
    Cursor *cursor;
    Screen *screen;
    Map *map;
} Core;

// Validator
#define VALIDATOR(cap_field, iface_field, val_func) \
    { \
        .capability_flag = interfaces->capabilities.cap_field, \
        .interface = interfaces->iface_field, \
        .validate = val_func \
    }

typedef bool (*ValidateFunc)(const void *interface);

typedef struct InterfaceValidator {
    bool capability_flag;
    const void *interface;
    ValidateFunc validate;
} InterfaceValidator;


// Prototypes
// Core
Core init_core(Map *map, Cursor *cursor, Screen *screen);
void core_move(Core *core, Coords move);
void core_action(Core *core);
void core_update_screen(Core *core);
void core_global_move(Core *core, Coords move);
void core_validate_interfaces(Core *core);
void core_free(Core *core);

// Screen
Screen init_screen(void);
void print_screen(const Screen *screen);
void add_separator(Screen *screen, int y, int x, wchar_t *borders);
void fill_area(Screen *screen, int y, int x, int height, int width, wchar_t symbol);
void add_borders(Screen *screen, int y, int x, int height, int width, const wchar_t *borders);

// Cursor
Cursor init_cursor(void *start_object, Coords start_coords);
void print_cursor(Cursor *cursor, Screen *screen);

// Map
void map_move(Map *map, Coords move);
MapObject map_get_current_object(Map *map);

// Validator
bool validate_object_interfaces(const ObjectInterfaces *interfaces);

#endif
