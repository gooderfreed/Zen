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

#define FIELD_HEIGHT 19
#define FIELD_WIDTH 7

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
    Objects source;
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
    void (*select_cards)(void *, Coords, CardsContainer *);
    void (*get_cards)   (void *, CardsContainer *);
    void (*place_cards) (void *, Coords, CardsContainer *);
    bool (*can_place)   (void *, Coords, CardsContainer *);

    void (*place_cursor)(void *, Coords , Coords *);
    void (*move)        (void *, Coords *, Coords);

    bool is_drawable;
    void (*print)(void *, struct Screen *, const struct Cursor *);
} ObjectInterface;

typedef struct Cursor {
    Coords coords;
    CardsContainer cards;
    Objects subject;
} Cursor;

typedef struct {
    ObjectInterface interface;
    Card deck[DECK_SIZE + 1];
    Card *pointer;
} Deck;

typedef struct {
    ObjectInterface interface;
    Card *stock[CARD_SUITS][CARD_NUMERALS];
} Stock;

typedef struct {
    ObjectInterface interface;
    Card *field[FIELD_HEIGHT][FIELD_WIDTH];
} Field;

typedef struct Screen {
    char *background[SCREEN_HEIGHT][SCREEN_WIDTH];
    wchar_t     data[SCREEN_HEIGHT][SCREEN_WIDTH];
    char *foreground[SCREEN_HEIGHT][SCREEN_WIDTH];
} Screen;

typedef struct {
    void *objects[OBJECTS_COUNT];
    Cursor *cursor;
    Screen *screen;
} Core;


// #define MAP_HEIGHT 1
// #define MAP_WIDTH 3

// typedef struct
// {
//     int size;
//     Objects *array;
// } MapObject;

// typedef MapObject **Map[MAP_HEIGHT][MAP_WIDTH];


//deck
Deck generate_deck(void);
void next_card(Deck *deck);
void reset_deck(Deck *deck);
Card *draw_card(Deck *deck);
void shuffle_deck(Deck *deck);
void print_deck(void *deck_pointer, Screen *screen, const Cursor *cursor);
Card *get_card_in_deck(void *deck_pointer, Cursor *cursor);
void place_cursor_in_deck(const Cursor *cursor, Coords *coords);
Card *select_card_in_deck(void *deck_pointer, Coords cursor_coords);
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
void move_cursor(Cursor *cursor, Coords move);
void print_cursor(Cursor *cursor, Screen *screen, void *target_struct);
Cursor init_cursor(Objects start_object, Coords start_coords);
Coords try_move(const Cursor *cursor, int delta_x, int delta_y);
//cursor

#endif
