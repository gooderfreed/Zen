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

#define CARD_SUITS 4
#define CARD_NUMERALS 13

#define CARD_HEIGHT 7
#define CARD_WIDTH 8

#define BORDER_OFFSET_X 1
#define BORDER_OFFSET_Y 1

#define DECK_OFFSET CARD_HEIGHT + 2
#define DECK_SIZE CARD_SUITS*CARD_NUMERALS

#define FIELD_HEIGHT 19
#define FIELD_WIDTH 7

#define SCREEN_HEIGHT 46 + DECK_OFFSET
#define SCREEN_WIDTH 58

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
    Null = 0,
    Ace = 1,
    Two, Three, Four, Five, Six, Seven, Eight, Nine,
    Ten, Jack, Queen, King
} Numeral;

typedef struct {
    int y;
    int x;
    int z;
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

typedef struct {
    Card deck[DECK_SIZE + 1];
    Card *pointer;
} Deck;

typedef Card *Stock[CARD_SUITS][CARD_NUMERALS];

typedef Card *Field[FIELD_HEIGHT][FIELD_WIDTH];

typedef struct {
    char *background[SCREEN_HEIGHT][SCREEN_WIDTH];
    wchar_t     data[SCREEN_HEIGHT][SCREEN_WIDTH];
    char *foreground[SCREEN_HEIGHT][SCREEN_WIDTH];
} Screen;

typedef struct {
    Coords coords;
    Card *card;
    Objects subject;
    void *objects[OBJECTS_COUNT];
} Cursor;


//deck
Deck generate_deck(void);
void shuffle_deck(Deck *deck);
Card *draw_card(Deck *deck);
void next_card(Deck *deck);
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


#endif
