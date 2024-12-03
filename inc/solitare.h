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

#ifndef SOLITARE_H
#define SOLITARE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

#define KEY_ESC L''

#define CARD_NUMERALS 13

#define CARD_SCALE_FACTOR 7
#define CARD_COVERED_HEIGHT 2

#define CARD_HEIGHT CARD_SCALE_FACTOR
#define CARD_WIDTH (CARD_SCALE_FACTOR + 1)

#define BORDER_OFFSET_X 1
#define BORDER_OFFSET_Y 1

#define DECK_OFFSET (CARD_HEIGHT + 2 * BORDER_OFFSET_Y)
#define DECK_SIZE (CARD_SUITS * CARD_NUMERALS)
#define DECK_DEFAULT_COORDS (Coords){.x = 0, .y = 0}

#define FIELD_HEIGHT 19
#define FIELD_WIDTH 7
#define FIELD_DEFAULT_COORDS (Coords){.x = 3, .y = 3}

#define STOCK_X_BASE (SCREEN_WIDTH - BORDER_OFFSET_X - 4 * CARD_WIDTH)
#define STOCK_AREA_HEIGHT (DECK_OFFSET - 2)
#define STOCK_AREA_WIDTH (4 * CARD_WIDTH)
#define STOCK_CURSOR_BASE_X (STOCK_X_BASE + (CARD_WIDTH / 2 - 1))
#define STOCK_DEFAULT_COORDS (Coords){.x = 2, .y = 0}

#define SCREEN_HEIGHT (2 * (FIELD_HEIGHT - 1) + CARD_HEIGHT + DECK_OFFSET + BORDER_OFFSET_X)
#define SCREEN_WIDTH (FIELD_WIDTH * CARD_WIDTH + 2 * BORDER_OFFSET_X)

#define clear() wprintf(L"\033[H\033[J")
#define gotoxy(x,y) wprintf(L"\033[%d;%dH", (y), (x))


typedef enum {
    Spades,
    Hearts,
    Clubs,
    Diamonds,
    CARD_SUITS
} Suit;

typedef enum {
    Ace = 1,
    Two, Three, Four, Five, Six, Seven, Eight, Nine,
    Ten, Jack, Queen, King,
} Numeral;

typedef struct {
    short y;
    short x;
    short z;
} Coords;

typedef enum{
    Deck_enum = 0,
    Field_enum,
    Stock_enum,
    OBJECTS_COUNT,
    Unknown
} Objects;

typedef struct {
    Coords coords;
    Numeral numeral;
    Suit suit;
    Objects object;
    bool selected;
} Card;

typedef struct {
    int size;
    void *source;
    Card *container[CARD_NUMERALS];
} CardsContainer;


#define CURSOR_UP    (Coords){.x = 0,  .y = -1}
#define CURSOR_DOWN  (Coords){.x = 0,  .y = 1}
#define CURSOR_LEFT  (Coords){.x = -1, .y = 0}
#define CURSOR_RIGHT (Coords){.x = 1,  .y = 0}
#define CURSOR_STAY  (Coords){.x = 0,  .y = 0}


struct Screen;
struct Cursor;

typedef struct {
    void (*print)(void *, struct Screen *, const struct Cursor *);
} Drawable;

typedef struct {
    void (*place_cursor)(void *, Coords , Coords *);
    void (*move)        (void *, Coords *, Coords);
} Interactable;

typedef struct {
    bool can_give_cards : 1;
    bool can_take_cards : 1;

    void (*select_cards)(void *, Coords, CardsContainer *);
    void (*get_cards)   (void *, CardsContainer *);
    void (*place_cards) (void *, Coords, CardsContainer *);
    bool (*can_place)   (void *, Coords, CardsContainer *);
} CardHandler;

typedef struct {
    bool (*is_button_position)(void *, Coords);
    void (*handle_button)(void *, Coords);
} ButtonHandler;

typedef struct {
    struct {
        bool can_hold_cards  : 1;
        bool is_drawable     : 1;
        bool is_interactable : 1;
        bool have_buttons    : 1;
    } capabilities;

    const Drawable *drawable;
    const Interactable *interactable;
    const CardHandler *card_handler;
    const ButtonHandler *button_handler;
} ObjectInterfaces;

typedef struct Cursor {
    Coords coords;
    CardsContainer cards;
    void *subject;
} Cursor;

typedef struct {
    ObjectInterfaces interfaces;
    Card deck[DECK_SIZE + 1];
    Card *pointer;
} Deck;

typedef struct {
    ObjectInterfaces interfaces;
    Card *stock[CARD_SUITS][CARD_NUMERALS];
} Stock;

typedef struct {
    ObjectInterfaces interfaces;
    Card *field[FIELD_HEIGHT][FIELD_WIDTH];
} Field;

typedef struct Screen {
    char *background[SCREEN_HEIGHT][SCREEN_WIDTH];
    wchar_t     data[SCREEN_HEIGHT][SCREEN_WIDTH];
    char *foreground[SCREEN_HEIGHT][SCREEN_WIDTH];
} Screen;

#define MAP_HEIGHT 1
#define MAP_WIDTH 3

typedef struct {
    void *object;
    Coords default_coords;
} MapObject;

typedef struct {
    MapObject objects[MAP_HEIGHT][MAP_WIDTH];
    Coords global_coords;
} Map;

typedef struct {
    Cursor *cursor;
    Screen *screen;
    Map *map;
} Core;


//deck
Deck generate_deck(void);
void next_card(Deck *deck);
void reset_deck(Deck *deck);
Card *draw_card(Deck *deck);
void shuffle_deck(Deck *deck);
void print_deck(void *deck_pointer, Screen *screen, const Cursor *cursor);
void move_in_deck(void *deck_pointer, Coords *coords, Coords delta);
void place_cursor_in_deck(void *deck_pointer, Coords cursor_coords, Coords *target_coords);
bool is_deck_button_position(void *deck_pointer, Coords coords);
void handle_deck_button(void *deck_pointer, Coords coords);
void select_card_in_deck(void *deck_pointer, Coords cursor_coords, CardsContainer *container);
void get_card_in_deck(void *deck_pointer, CardsContainer *container);
//deck

//screen
Screen init_screen(void);
void print_screen(const Screen *screen);
void add_separator(Screen *screen, int y, int x, wchar_t *borders);
void fill_area(Screen *screen, int y, int x, int height, int width, wchar_t symbol);
void colorize_area(Screen *screen, const Card *card, int y, int x, int height, int width);
void add_borders(Screen *screen, int y, int x, int height, int width, const wchar_t *borders);
//screen

//field
Field init_field(Deck *deck);
void select_column(Field *field, Coords coords);
void print_cursor_in_field(const Cursor *cursor, Coords *coords);
void print_field(void *field_pointer, Screen *screen, const Cursor *hovered_card);
void move_in_field(void *field_pointer, Coords *coords, Coords delta);
void place_cursor_in_field(void *field_pointer, Coords cursor_coords, Coords *target_coords);

void get_cards_in_field(void *field_pointer, CardsContainer *container);
bool can_place_in_field(void *field_pointer, Coords cursor_coords, CardsContainer *container);
void place_cards_in_field(void *field_pointer, Coords cursor_coord, CardsContainer *container);
void select_cards_in_field(void *field_pointer, Coords cursor_coords, CardsContainer *container);
//field

//card
extern wchar_t fat_border[8];
extern wchar_t card_border[8];
wchar_t suit_to_text(Suit suit);
char *numeral_to_text(Numeral numeral);
void print_card(Screen *screen, const Card *card, int y, int x, int size_y, int size_x);
//card

//cursor
void print_cursor(Cursor *cursor, Screen *screen);
Cursor init_cursor(void *start_object, Coords start_coords);
//cursor

//stock
Stock init_stock(void);
void print_stock(void *stock_pointer, Screen *screen, const Cursor *cursor);
void move_in_stock(void *stock_pointer, Coords *coords, Coords delta);
void place_cursor_in_stock(void *stock_pointer, Coords cursor_coords, Coords *target_coords);
//stock 

// Interface validation
bool validate_object_interfaces(const ObjectInterfaces *interfaces);

// Добавим новые типы для валидации
typedef bool (*ValidateFunc)(const void *interface);

typedef struct {
    bool capability_flag;       // Флаг из capabilities
    const void *interface;      // Указатель на интерфейс
    ValidateFunc validate;      // Функция валидации
} InterfaceValidator;

#endif
