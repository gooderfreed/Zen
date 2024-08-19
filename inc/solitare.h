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

#define KEY_ESC L''

#define CARD_SUITS 4
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

#define OBJECTS_COUNT 3

#define clear() wprintf(L"\033[H\033[J")
#define gotoxy(x,y) wprintf(L"\033[%d;%dH", (y), (x))


typedef enum {
    Spades,
    Hearts,
    Clubs,
    Diamonds
} Suit;

typedef enum {
    Ace = 1,
    Two, Three, Four, Five, Six, Seven, Eight, Nine,
    Ten, Jack, Queen, King
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
    Unknown
} Objects;

typedef struct {
    Coords coords;
    Numeral numeral;
    Suit suit;
    Objects object;
    bool selected;
} Card;

// typedef struct {
//     Card deck[DECK_SIZE + 1];
//     Card *pointer;
// } Deck;

// typedef void (*Func)(void *);
// typedef void (*GetCard)(void *);
// typedef void (*place_card)(void *);

typedef struct {
    Coords coords;
    Card *card;
    Objects subject;
    void *objects[OBJECTS_COUNT];
} Cursor;

typedef struct {
    Card deck[DECK_SIZE + 1];
    Card *pointer;

    // Func select_card;
    // Func get_card;
    // Func place_card;
} Deck;

// typedef Card *Stock[CARD_SUITS][CARD_NUMERALS];
typedef struct {
    Card *stock[CARD_SUITS][CARD_NUMERALS];
} Stock;

// typedef Card *Field[FIELD_HEIGHT][FIELD_WIDTH];
typedef struct {
    Card *field[FIELD_HEIGHT][FIELD_WIDTH];

    // Func select_card;
    // Func get_card;
    // Func place_card;
} Field;

typedef struct {
    char *background[SCREEN_HEIGHT][SCREEN_WIDTH];
    wchar_t     data[SCREEN_HEIGHT][SCREEN_WIDTH];
    char *foreground[SCREEN_HEIGHT][SCREEN_WIDTH];
} Screen;


//deck
Deck generate_deck(void);
Card *draw_card(Deck *deck);
void next_card(Deck *deck);
void shuffle_deck(Deck *deck);
void reset_deck(Deck *deck);
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
int get_last_card_y(const Field *field, int x);
void select_column(Field *field, Cursor *cursor);
void print_cursor_in_field(const Cursor *cursor, Coords *coords);
void print_field(Screen *screen, const Field *field, const Cursor *hovered_card);
void move_in_field(Coords *res_coords, const Cursor *cursor, int delta_x, int delta_y);
//field

//card
extern wchar_t fat_border[8];
extern wchar_t card_border[8];
wchar_t suit_to_text(Suit suit);
char *numeral_to_text(Numeral numeral);
void print_card(Screen *screen, const Card *card, int y, int x, int size_y, int size_x);
//card

//cursor
void print_cursor(Screen *screen, const Cursor *cursor);
Cursor init_cursor(Deck *deck, Field *field, Stock *stock);
void move_cursor(Cursor *cursor, int delta_x, int delta_y);
Coords try_move(const Cursor *cursor, int delta_x, int delta_y);
//cursor

#endif
